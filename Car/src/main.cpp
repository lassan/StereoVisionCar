#include "../header/includeFiles.h"
#include "../header/helperfunctions.h"
#include "../header/Stereo.h"
#include "../header/Car.h"
#include "../header/main.h"

Mat _M1, _D1, _M2, _D2, _R1, _R2, _P1, _P2, _Q;

int _nOfDisparitiesCoefficient;
StereoBM _sbm;
VideoCapture _leftCamera, _rightCamera;
vector<Mat> _leftFrames, _rightFrames;
Mat _leftCameraMap1, _leftCameraMap2, _rightCameraMap1, _rightCameraMap2;
Rect _imageSegment(40, 80, 280, 120);

#define PORT_send 8800
#define PORT_recv 9900
#define Channel1 0
#define Channel2 1
#define Normal 0
#define Blob 1
#define Disparity 2
int is_data_ready = 0;
int serversock, clientsock;
int hasClient = 0;

int sockfd, recvsock, listensock;
struct sockaddr_in servaddr;
struct sockaddr_in server;
FLAGS::CLIENTDISPLAY clientDisplay = FLAGS::LEFT;
/*Variables for timing*/
int MAX_TIMING_ITERATIONS = 100;

int currentChannel = 0;
int currentMode = 0;
int warnCode = 2;

#define MIN_NDISPARITY 3
#define MAX_NDISPARITY 8

#define OMPTHREADS 3

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
bool _serverEnabled = false; //true: requires client availability
bool _drivingEnabled = true;

/*control changing disparity*/
bool _changeNdisparity = false;
bool _ndispInc = false;

/*byte array to send to the car*/
unsigned char _speed[13];
bool _carMoving = true;

Stereo _stereo(MIN_NDISPARITY, 21);
Car _car;

void Initialise()
{
    /*Set number of threads to 3*/
    omp_set_nested(1);
    omp_set_num_threads(OMPTHREADS);

    if (_serverEnabled)
    {
        cout << "Initialising server. Please start the client" << endl;
        InitServer();
        cout << "InitDirectionsFromClient. Please start the client" << endl;
        InitDirectionsFromClient();
    }
    if (_drivingEnabled)
        _car.driveUnsafe(0, 0);

    /*Load calibration matrices from file*/
    cout << "Loading camera calibration data" << endl;
    InitCalibrationData();

    /*Open camera streams*/
    cout << "Initialising stereo cameras" << endl;
    InitCameras();

    /*Generate Superimposed images*/
    //    cout << "Generating superimposed image sample" << endl;
    //    GenerateSuperImposedImages();
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

void InitDirectionsFromClient()
{

    /* open socket */
    if ((listensock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        cerr << "socket() failed";
        exit(1);
    }

    /* setup server's IP and port */
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT_recv);
    server.sin_addr.s_addr = INADDR_ANY;

    /* bind the socket */
    if (bind(listensock, (const sockaddr*) &server, sizeof(server)) == -1)
    {
        cerr << "bind() failed";
        exit(1);
    }

    /* wait for connection */
    if (listen(listensock, 2) == -1)
    {
    }

    /* accept a client */
    if ((recvsock = accept(listensock, NULL, NULL)) == -1)
    {
        cerr << "accept() failed";
        exit(1);
    }

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

void CarDrivingWorker()
{
    while (true)
    {

        /* select if data available*/
        int n;
        fd_set input, used;
        struct timeval timeout;

        /* Initialize the input set */
        FD_ZERO(&input);
        FD_SET(recvsock, &input);

        /* Initialize the timeout structure */
        timeout.tv_sec = 0;
        timeout.tv_usec = 1;

        used = input;

        /* Do the select */
        n = select(recvsock + 1, &used, NULL, NULL, &timeout);

        /* See if there was an error */
        if (n < 0)
        {
            perror("select failed");
        }
        else if (n == 0)
        {
//			puts("TIMEOUT");
        }
        else
        {
            if (FD_ISSET(recvsock, &used))
            {
                char buffer[100];
                recv(recvsock, buffer, sizeof(buffer), 0);
                tutorial::Packet packetHeader;
                packetHeader.ParseFromArray(buffer, sizeof(buffer));
                if (packetHeader.has_directionx()
                        && packetHeader.has_directiony())
                {
                    cout << "X: " << packetHeader.directionx();
                    cout << "\tY: " << packetHeader.directiony() << endl;
                        _car.driveSafe(packetHeader.directionx(),
                                packetHeader.directiony(),
                                _stereo.getClosestObjectVal(),_stereo.getVisualInfo());
                }
            }
        }
    }
}

void ClientDisplay(StereoPair &input, Mat &image)
{
    if (hasClient)
    {
        switch (clientDisplay)
        {
            case FLAGS::LEFT:
                SendDataToClient(input.leftImage);
                break;
            case FLAGS::RIGHT:
                SendDataToClient(input.rightImage);
                break;
            case FLAGS::DISPARITY:
                SendDataToClient(image);
                break;
            default:
                break;
        }
    }
    else
    {
        _car.brake(); //stop the car if there is no client.
        ListenForClient();
    }
}

void ImageAcquisitionWorker()
{
    int iterationCounter = 0;
    float iterationTime = 0;
    float totalTime = 0;

    while (1)
    {
        iterationTime = getTickCount();

        cout << "highest: " << _stereo.getClosestObjectVal() << "\tnumber: " << _stereo.getNumObjects() << endl;
#pragma omp critical(buffer0)
        {
            if (_buffer0Processed)
            {

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


                    imshow("disp", _disparityBuffer.leftImage);
                    waitKey(100);
                }

                GetStereoImages(_buffer0);

                imshow("img", _buffer0.leftImage);
                waitKey(5);

                if (_serverEnabled)
                    ClientDisplay(_buffer0, _disparityBuffer.leftImage);

                _buffer0Processed = false;

                iterationCounter++;
            }
        }
#pragma omp critical(buffer1)
        {

            if (_buffer1Processed)
            {
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

                    imshow("disp", _disparityBuffer.rightImage);
                    waitKey(100);

                }

                GetStereoImages(_buffer1);

                if (_serverEnabled)
                    ClientDisplay(_buffer1, _disparityBuffer.rightImage);

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
        iterationTime = getTickCount();

//        _car.driveSafe(0, 1, _stereo.getClosestObjectVal(),_stereo.getVisualInfo());

#pragma omp critical(buffer1)
        {
            if (!_buffer1Processed)
            {
                _stereo.changeParameters(21, _car);

                _disparityBuffer.rightImage = _stereo.disparityMap(_buffer1);
                _buffer1Processed = true;

                iterationCounter++;
            }
        }
#pragma omp critical(buffer0)
        {
            if (!_buffer0Processed)
            {
                _stereo.changeParameters(21, _car);

                _disparityBuffer.leftImage = _stereo.disparityMap(_buffer0);
                _buffer0Processed = true;

                iterationCounter++;
            }
        }

        iterationTime = (getTickCount() - iterationTime) * 0.000000001;
        totalTime += iterationTime;
        if (iterationCounter >= MAX_TIMING_ITERATIONS)
        {
            cout << "Image processing (fps): "
                    << (iterationCounter * 2) / totalTime << endl;
            iterationCounter = 0;
            totalTime = 0;
        }
    }
}

int main()
{
    Initialise();
//    destroyAllWindows();

    int tid;

#pragma omp parallel
    {

        tid = omp_get_thread_num();

        if (tid == 0)
        {
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
                        //Have thread for driving instructions here
                        if (_serverEnabled)
                            CarDrivingWorker();
                    }
                }

            }
        }
        else if (tid == 1)
        {
            DisparityCalculationWorker();
        }
    }
    return 0;
}

void InitServer()
{
    struct sockaddr_in server;

    /* open socket */
    if ((serversock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        cerr << "socket() failed";
        exit(1);
    }

    /* setup server's IP and port */
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT_send);
    server.sin_addr.s_addr = INADDR_ANY;

    /* bind the socket */
    if (bind(serversock, (const sockaddr*) &server, sizeof(server)) == -1)
    {
        //quit("bind() failed", 1);
        cerr << "bind() failed";
        exit(1);
    }

    /* wait for connection */
    if (listen(serversock, 2) == -1)
    {
    }

    /* accept a client */
    if ((clientsock = accept(serversock, NULL, NULL)) == -1)
    {
        cerr << "accept() failed";
        exit(1);
    }
    hasClient = 1;

}

void ListenForClient()
{
    /* wait for connection */
    if (listen(serversock, 2) == -1)
    {
    }

    /* accept a client */
    if ((clientsock = accept(serversock, NULL, NULL)) == -1)
    {
        cerr << "accept() failed";
        exit(1);
    }
    hasClient = 1;
}

void SendDataToClient(Mat &image)
{
    IplImage *img1 = new IplImage(image);

    int bytes = 0;

    bytes = send(clientsock, img1->imageData, img1->imageSize, 0);

    /* if something went wrong, restart the connection */
    if (bytes != img1->imageSize)
    {
        fprintf(stderr, "Connection closed.\n");
        close(clientsock);
        hasClient = 0;
    }
//	cvReleaseImage(&(img1));
}

void checkForData()
{
    int n = 0;
    fd_set input;
    struct timeval timeout;

    FD_ZERO(&input);
    FD_SET(clientsock, &input);

    timeout.tv_sec = 0;
    timeout.tv_usec = 1;

    n = select(clientsock + 1, &input, NULL, NULL, &timeout);
    /* start sending images */
    if (n < 0) {
        perror("select failed");
    } else if (n == 0) {

    } else if (FD_ISSET(clientsock,&input)) {
        char buffer[32];
        recv(clientsock, buffer, sizeof(buffer), 0);
        tutorial::Packet header;
        header.ParseFromArray(buffer, sizeof(buffer));
        if (header.has_channel()) {
            switch (header.channel()) {
            case Channel1:
                currentChannel = Channel1;
                break;
            case Channel2:
                currentChannel = Channel2;
                break;
            default:
                break;
            }

        } else if(header.has_headertype()) {
            switch (header.headertype()) {
            case Normal:
                clientDisplay = FLAGS::LEFT;
                break;
            case Blob:
                clientDisplay = FLAGS::BLOBS;
                destroyAllWindows();
                break;
            case Disparity:
                clientDisplay = FLAGS::DISPARITY;
                destroyAllWindows();
                break;
            default:
                break;
            }
        }
    }
}
