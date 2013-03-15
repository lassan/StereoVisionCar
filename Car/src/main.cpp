#include "../header/includeFiles.h"
#include "../header/helperfunctions.h"
#include "../header/Server.h"
#include "../header/Stereo.h"
#include "../header/Car.h"
#include "../header/main.h"
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <stdlib.h>

Mat _M1, _D1, _M2, _D2, _R1, _R2, _P1, _P2, _Q;

int _nOfDisparitiesCoefficient;
StereoBM _sbm;
VideoCapture _leftCamera, _rightCamera;
vector<Mat> _leftFrames, _rightFrames;
Mat _leftCameraMap1, _leftCameraMap2, _rightCameraMap1, _rightCameraMap2;
Rect _imageSegment(40, 60, 240, 120);

/*Variables for timing*/
int MAX_TIMING_ITERATIONS = 50;

#define OMPTHREADS 2

/*Variables for storing images*/
StereoPair _buffer0;
StereoPair _buffer1;
StereoPair _disparityBuffer;
bool _bufferEmpty = true;

/*Variables for synchronising workers*/
bool _buffer0Processed = true;
bool _buffer1Processed = true;
static bool _buffersFull = false;
bool _invalidateDispBufLeft = false;
bool _invalidateDispBufRight = false;

/*control visual output*/
bool _changeDisparityDynamically = false; //true: display color circle, don't display blobs/disparity
bool _displayBlobs = true; //true: display blobs; false: display disparity map
bool _serverEnabled = true; //true: requires client availability
bool _drivingEnabled = true;

string _messageToSend = "";
bool _override = true;

Server _server;
Stereo _stereo(21);
Car _car;

void Initialise()
{
    omp_set_num_threads(OMPTHREADS);

    if (_serverEnabled)
    {
        _server.initialise();
    }

    /*Load calibration matrices from file*/
    cout << "Loading camera calibration data" << endl;
    InitCalibrationData();

    /*Open camera streams*/
    cout << "Initialising stereo cameras" << endl;
    InitCameras();
}

void InitCalibrationData()
{
    string filePath = "CalibrationMatrices320/";
    LoadMatrixFromFile(filePath, "M1", _M1);
    LoadMatrixFromFile(filePath, "D1", _D1);
    LoadMatrixFromFile(filePath, "M2", _M2);
    LoadMatrixFromFile(filePath, "D2", _D2);
    LoadMatrixFromFile(filePath, "R1", _R1);
    LoadMatrixFromFile(filePath, "R2", _R2);
    LoadMatrixFromFile(filePath, "P1", _P1);
    LoadMatrixFromFile(filePath, "P2", _P2);
    LoadMatrixFromFile(filePath, "Q", _Q);
}

void InitCameras()
{
    /*Create rectification matrices*/
    initUndistortRectifyMap(_M1, _D1, _R1, _P1, Size(320, 240), CV_16SC2,
            _leftCameraMap1, _leftCameraMap2);
    initUndistortRectifyMap(_M2, _D2, _R2, _P2, Size(320, 240), CV_16SC2,
            _rightCameraMap1, _rightCameraMap2);

    _leftCamera.open(2);
    _rightCamera.open(1);

    if (_leftCamera.isOpened())
    {
        cout << "Left camera stream opened" << endl;
    }
    else
    {
        cout << "Left  camera stream failed to open. Terminating" << endl;
        abort();
    }

    if (_rightCamera.isOpened())
    {
        cout << "Right camera stream opened" << endl;
    }
    else
    {
        cout << "Right camera stream failed to open. Terminating" << endl;
        abort();
    }

    _leftCamera.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    _leftCamera.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

    _rightCamera.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    _rightCamera.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
}

void GenerateSuperImposedImages()
{
    namedWindow("Overlaid");

    StereoPair camImages;

    GetStereoImages(camImages);

    imshow("Right image", camImages.rightImage);
    imshow("Left image", camImages.leftImage);

    Mat overlay = OverlayImages(camImages, 0.5);
    imshow("Overlaid", overlay);

    cout << "Press any key to continue." << endl;
    waitKey(0);
}

void GetStereoImages(StereoPair &input)
{
    _leftCamera.grab();
    _rightCamera.grab();

    _leftCamera.retrieve(input.leftImage, 0);
    _rightCamera.retrieve(input.rightImage, 0);

    remap(input.leftImage, input.leftImage, _leftCameraMap1, _leftCameraMap2,
            INTER_LINEAR);
    remap(input.rightImage, input.rightImage, _rightCameraMap1,
            _rightCameraMap2, INTER_LINEAR);

    input.leftImage = input.leftImage(_imageSegment);
    input.rightImage = input.rightImage(_imageSegment);

    cvtColor(input.leftImage, input.leftImage, CV_RGB2GRAY);
    cvtColor(input.rightImage, input.rightImage, CV_RGB2GRAY);

    blur(input.leftImage, input.leftImage, Size(3, 3));
    blur(input.rightImage, input.rightImage, Size(3, 3));
}

void ImageAcquisitionWorker()
{
    int iterationCounter = 0;
    float iterationTime = 0;
    float totalTime = 0;

    double frameTime = 0;

    while (1)
    {
        iterationTime = getTickCount();

//        cout << "highest: " << _stereo.getClosestObjectVal()
//                << "\tnumber: " << _stereo.getNumObjects()
//                << "\tarea: " << _stereo.getClosestObjectArea()
//                << endl;

#pragma omp critical(buffer0)
        {
            if (_buffer0Processed)
            {
                frameTime = getTickCount();


                /*Blobs for disparity from image in buffer 0*/
                if (_disparityBuffer.leftImage.data != NULL)
                {
                    _stereo.detectObjects(_disparityBuffer.leftImage);

                    if (_invalidateDispBufLeft)
                    {
                        _invalidateDispBufLeft = false;
                    }
                    else if (_stereo.parameterChangeRequired())
                    {
                        _invalidateDispBufRight = true;
                    }
//
//                    imshow("disp", _disparityBuffer.leftImage);
//                    waitKey(5);
                }

                GetStereoImages(_buffer0);


                frameTime = (getTickCount() - frameTime)/getTickFrequency();

                _messageToSend = intToString(1/frameTime);
                if (_serverEnabled)
                    _server.sendData(_buffer0, _disparityBuffer.leftImage,_stereo.shouldBrake(),_car, _messageToSend);

                _buffer0Processed = false;

                iterationCounter++;
            }
        }
#pragma omp critical(buffer1)
        {

            if (_buffer1Processed)
            {

                frameTime = getTickCount();

                /*Blobs for disparity from image in buffer 1*/
                if (_disparityBuffer.rightImage.data != NULL)
                {
                    _stereo.detectObjects(_disparityBuffer.rightImage);

                    if (_invalidateDispBufRight)
                    {
                        _invalidateDispBufRight = false;
                    }
                    else if (_stereo.parameterChangeRequired())
                    {
                        _invalidateDispBufLeft = true;
                    }

//                    imshow("disp", _disparityBuffer.rightImage);
//                    waitKey(5);

                }

                GetStereoImages(_buffer1);


                frameTime = (getTickCount() - frameTime)/getTickFrequency();


                _messageToSend = intToString(1/frameTime);
                if (_serverEnabled)
                    _server.sendData(_buffer0, _disparityBuffer.leftImage,_stereo.shouldBrake(),_car, _messageToSend);

                _buffer1Processed = false;

                iterationCounter++;
            }
        }

        _buffersFull = true;

        iterationTime = (getTickCount() - iterationTime) * 0.000000001;
        totalTime += iterationTime;
        if (iterationCounter >= MAX_TIMING_ITERATIONS)
        {
            cout << "Image acquisition and preparation (fps): "
                    << (iterationCounter * 2) / totalTime << endl;
            iterationCounter = 0;
            totalTime = 0;
        }
    }
}

void DisparityCalculationWorker()
{
    int iterationCounter = 0;
    float iterationTime = 0;
    float totalTime = 0;

    /*Print reassuring message*/
    cout << "Waiting for buffers to fill.";
    while (!_buffersFull)
    {
        float tempTime = getTickCount();
        tempTime = (getTickCount() - tempTime) * 0.000000001;
        if (tempTime > 0.01)
            cout << ".";
    }
    cout << endl << "Buffers full. Commencing processing" << endl;

    while (true)
    {

#pragma omp critical(buffer1)
        {
            if (!_buffer1Processed)
            {
                _stereo.changeParameters(21, _car);

                _disparityBuffer.rightImage = _stereo.disparityMap(_buffer1);
                _buffer1Processed = true;
            }
        }
#pragma omp critical(buffer0)
        {
            if (!_buffer0Processed)
            {
                _stereo.changeParameters(21, _car);

                _disparityBuffer.leftImage = _stereo.disparityMap(_buffer0);
                _buffer0Processed = true;
            }
        }
    }
}

int main()
{
    Initialise();
//    destroyAllWindows();

#pragma omp parallel
    {
#pragma omp sections
        {
#pragma omp section
            {
                ImageAcquisitionWorker();
            }
#pragma omp section
            {
                DisparityCalculationWorker();
            }
        }

    }
    return 0;
}

