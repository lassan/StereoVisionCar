/*
 * main.cpp
 *
 *  Created on: 27 Nov 2012
 *      Author: hassan
 */

#include "../header/main.h"
#include "../header/helperfunctions.h"
#include "../header/ArduinoInterface.h"

Mat _M1, _D1, _M2, _D2, _R1, _R2, _P1, _P2, _Q;

int _nOfDisparitiesCoefficient;
StereoBM _sbm;
VideoCapture _leftCamera, _rightCamera;
vector<Mat> _leftFrames, _rightFrames;
Mat _leftCameraMap1, _leftCameraMap2, _rightCameraMap1, _rightCameraMap2;
Rect _imageSegment(0, 40, 320, 160);

#define PORT 9900
#define Channel1 0
#define Channel2 1
#define Normal 0
#define Blob 1
#define Disparity 2
int is_data_ready = 0;
int serversock, clientsock;
int hasClient = 0;

/*Variables for timing*/
int MAX_TIMING_ITERATIONS = 50;

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
bool _buffersFull = false;
bool _invalidateDispBufLeft = false;
bool _invalidateDispBufRight = false;

/*control visual output*/
bool _changeDisparityDynamically = false; //true: display color circle, don't display blobs/disparity
bool _displayBlobs = true; //true: display blobs; false: display disparity map
bool _serverEnabled = false; //true: requires client availability

/*control changing disparity*/
bool _changeNdisparity = false;
bool _ndispInc = false;

void Initialise()
{
    /*Set number of threads to 2*/
    omp_set_nested(1);
    omp_set_num_threads(OMPTHREADS);

    if (_serverEnabled)
    {
        cout << "Initialising server. Please start the client" << endl;
        InitServer();
    }

    /*Load calibration matrices from file*/
    cout << "Loading camera calibration data" << endl;
    InitCalibrationData();

    /*Open camera streams*/
    cout << "Initialising stereo cameras" << endl;
    InitCameras();

    /*Set default block matching parameters*/
    cout << "Initialising disparity map generator" << endl;
    InitStereoBM(MIN_NDISPARITY);

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
    //    LoadMatrixFromFile(filePath,"Q", _Q);
}

void InitCameras()
{
    /*Create rectification matrices*/
    initUndistortRectifyMap(_M1, _D1, _R1, _P1, Size(320, 240), CV_16SC2,
            _leftCameraMap1, _leftCameraMap2);
    initUndistortRectifyMap(_M2, _D2, _R2, _P2, Size(320, 240), CV_16SC2,
            _rightCameraMap1, _rightCameraMap2);

    _leftCamera.open(1);
    _rightCamera.open(0);

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

//If flag is ASOLUTE, set nDisparity to 16 * disp, else increment/decrement
//current disparity by 16 * disp
void InitStereoBM(int disp, FLAGS::NUMDISPARITY flag, int _SADWindowSize)
{
    if (flag == FLAGS::ABSOLUTE)
        _nOfDisparitiesCoefficient = disp;
    else if (flag == FLAGS::INCREMENT)
        _nOfDisparitiesCoefficient += disp;
    else if (flag == FLAGS::DECREMENT)
        _nOfDisparitiesCoefficient -= disp;

    _sbm = StereoBM(StereoBM::BASIC_PRESET, 16 * _nOfDisparitiesCoefficient,
            _SADWindowSize);

    cout << "nDisparity set to " << intToString(_nOfDisparitiesCoefficient)
            << ". Window size set to " << intToString(_SADWindowSize) << endl;
}

void GenerateSuperImposedImages()
{
    namedWindow("Overlaid");

    StereoPair camImages;

    _leftCamera.grab();
    _leftCamera.retrieve(camImages.leftImage, 0);

    _rightCamera.grab();
    _rightCamera.retrieve(camImages.rightImage, 0);

    PrepareStereoPairForDisparityCalculation(camImages);

    //    imshow("Right image", camImages.rightImage);
    //    imshow("Left image", camImages.leftImage);

    Mat overlay = OverlayImages(camImages, 0.5);
    imshow("Overlaid", overlay);

    cout << "Press any key to continue." << endl;
    waitKey(0);
}

void PrepareStereoPairForDisparityCalculation(StereoPair &input)
{
    remap(input.leftImage, input.leftImage, _leftCameraMap1, _leftCameraMap2,
            INTER_LINEAR);
    remap(input.rightImage, input.rightImage, _rightCameraMap1,
            _rightCameraMap2, INTER_LINEAR);

    input.leftImage = input.leftImage(_imageSegment);
    input.rightImage = input.rightImage(_imageSegment);

    cvtColor(input.leftImage, input.leftImage, CV_RGB2GRAY);
    cvtColor(input.rightImage, input.rightImage, CV_RGB2GRAY);

//	blur(input.leftImage, input.leftImage, Size(3, 3));
//	blur(input.rightImage, input.rightImage, Size(3, 3));
}

Mat CalculateDisparityBM(StereoPair const & images)
{
    int rows = images.leftImage.rows;
    int cols = images.leftImage.cols;

    Mat imgDisparity16S = Mat(rows, cols, CV_16S);
    Mat imgDisparity8U = Mat(rows, cols, CV_8UC1);

    //Calculate the disparity image
    _sbm(images.leftImage, images.rightImage, imgDisparity16S, CV_16S);

    //Check the extreme values
    double minVal;
    double maxVal;
    minMaxLoc(imgDisparity16S, &minVal, &maxVal);

    //Prepare for display
    imgDisparity16S.convertTo(imgDisparity8U, CV_8UC1, 255 / (maxVal - minVal));

    return imgDisparity8U;
}

/*returns true if close obstacle found, else returns false*/
bool FindBlobs(Mat &bm)
{
    Mat element(3, 3, CV_8U, cv::Scalar(1));

    Mat displayImage;

    if (_displayBlobs)
        erode(bm, bm, element);

    IplImage *dispIpl = new IplImage(bm); //create an IplImage from Mat

    //Declare variables
    CBlobResult blobs;
//	CBlob *currentBlob;
    int minArea = 125;
//	false
    blobs = CBlobResult(dispIpl, NULL, 0); //get all blobs in the disparity map
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, minArea); //filter blobs by area and remove all less than minArea

    //Display blobs
    IplImage *displayedImage = cvCreateImage(bm.size(), 8, 3); //create image for outputting blobs

    vector<int> meanPixelValues;

    for (int i = 0; i < blobs.GetNumBlobs(); i++)
    {
        meanPixelValues.push_back(blobs.GetBlob(i)->Mean(dispIpl));
    }

    if (_displayBlobs)
    {
        for (int i = 0; i < blobs.GetNumBlobs(); i++)
        {
            Scalar color = GetBlobColor(meanPixelValues[i]);
            blobs.GetBlob(i)->FillBlob(displayedImage, color);
        }
        displayImage = displayedImage; //Convert to Mat for use in imshow()
        imshow("Blobs", displayImage);
        waitKey(5);
    }

    cvReleaseImage(&displayedImage);
    vector<int>::const_iterator it;
    if (meanPixelValues.size() > 0)
    {
        it = max_element(meanPixelValues.begin(), meanPixelValues.end());

        if (*it >= (16 * 15))
        {
            if (_nOfDisparitiesCoefficient <= MAX_NDISPARITY)
            {
                _changeNdisparity = true;
                _ndispInc = true;
                if (_changeDisparityDynamically)
                    return true;
                else
                    return false;
            }
        }
        else if (*it < (16 * 10))
        {
            if (_nOfDisparitiesCoefficient > MIN_NDISPARITY)
            {
                _changeNdisparity = true;
                _ndispInc = false;
                if (_changeDisparityDynamically)
                    return true;
                else
                    return false;
            }
        }
    }
    return false;
}

Mat canvas = Mat(320, 320, CV_32FC3); //for displaying feedback when disparity is changing
void ChangeDisparityDynamically()
{

    if (_changeNdisparity)
    {
        if (_ndispInc) //slow down/ increase disparity
        {
            if (_nOfDisparitiesCoefficient == MAX_NDISPARITY)
            {
                //Stop car
                circle(canvas, Point(160, 160), 100, CV_RGB(255,0,0), -1);
                cout << "Stop the car!" << endl;
                warnCode = 0;
            }
            else
            {
                InitStereoBM(1, FLAGS::INCREMENT);

                circle(canvas, Point(160, 160), 100, CV_RGB(255,140,0), -1);
                cout << "Slow down!" << endl;
                warnCode = 1;
            }
        }
        else if (!_ndispInc)
        {
            if (_nOfDisparitiesCoefficient > MIN_NDISPARITY)
            {
                InitStereoBM(1, FLAGS::DECREMENT);

                circle(canvas, Point(160, 160), 100, CV_RGB(0,128,0), -1);
                cout << "Speed up!" << endl;
                warnCode = 2;
            }
        }
        _changeNdisparity = false;
    }
    else
    {
//        circle(canvas, Point(160, 160), 100, CV_RGB(0,128,0), -1);
    }

    imshow("canvas", canvas);
    waitKey(20);

}

void ImageAcquisitionWorker()
{
    int iterationCounter = 0;
    float iterationTime = 0;
    float totalTime = 0;

    while (1)
    {

        iterationTime = getTickCount();

#pragma omp critical(buffer0)
        {

            if (_buffer0Processed)
            {

                /*Blobs for disparity from image in buffer 0*/
                if (_disparityBuffer.leftImage.data != NULL)
                {
                    if (_invalidateDispBufLeft)
                    {
                        _invalidateDispBufLeft = false;
                    }
                    else if (FindBlobs(_disparityBuffer.leftImage))
                    {
                        _invalidateDispBufRight = true;
                    }
                    if (!_changeDisparityDynamically && !_displayBlobs)
                    {
                        imshow("disparity", _disparityBuffer.leftImage);
                        waitKey(2);
                    }

                }

                _leftCamera.grab();
                _rightCamera.grab();

                _leftCamera.retrieve(_buffer0.leftImage, 0);
                _rightCamera.retrieve(_buffer0.rightImage, 0);

                PrepareStereoPairForDisparityCalculation(_buffer0);

                /*Server stuff*/
                if (_serverEnabled)
                {
                    if (hasClient)
                    {
                        if (currentMode == Normal)
                        {
                            if (currentChannel == Channel1)
                            {
                                SendDataToServer(_buffer0.leftImage);
                            }
                            else
                            {
                                SendDataToServer(_buffer0.rightImage);
                            }
                        }
                        else
                            SendDataToServer(_disparityBuffer.leftImage);
                        checkForData();
                    }
                    else
                    {
                        ListenForClient();
                    }
                }
                /*End server stuff*/

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
                    if (_invalidateDispBufRight)
                    {
                        _invalidateDispBufRight = false;
                    }
                    else if (FindBlobs(_disparityBuffer.rightImage))
                    {
                        _invalidateDispBufLeft = true;
                    }
                    if (!_changeDisparityDynamically && !_displayBlobs)
                    {
                        imshow("disparity", _disparityBuffer.rightImage);
                        waitKey(2);
                    }
                }

                _leftCamera.grab();
                _rightCamera.grab();

                _leftCamera.retrieve(_buffer1.leftImage, 0);
                _rightCamera.retrieve(_buffer1.rightImage, 0);

                PrepareStereoPairForDisparityCalculation(_buffer1);

                /*Server stuff*/
                if (_serverEnabled)
                {
                    if (hasClient)
                    {
                        if (currentMode == Normal)
                        {
                            switch (currentChannel)
                            {
                            case Channel1:
                                SendDataToServer(_buffer0.leftImage);
                                break;
                            default:
                                SendDataToServer(_buffer0.rightImage);
                                break;
                            }
                        }
                        else
                            SendDataToServer(_disparityBuffer.rightImage);
                        checkForData();
                    }
                    else
                    {
                        ListenForClient();
                    }
                }
                /*End server stuff*/

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

#pragma omp critical(buffer1)
        {

            if (_changeDisparityDynamically)
                ChangeDisparityDynamically();

            if (!_buffer1Processed)
            {

                _disparityBuffer.rightImage = CalculateDisparityBM(_buffer1);

                _buffer1Processed = true;

                iterationCounter++;
            }
        }

#pragma omp critical(buffer0)
        {

            if (_changeDisparityDynamically)
                ChangeDisparityDynamically();

            if (!_buffer0Processed)
            {

                _disparityBuffer.leftImage = CalculateDisparityBM(_buffer0);

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
    destroyAllWindows();

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

                    }
                }

            }
        }
        else if (tid == 1)
            DisparityCalculationWorker();
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
    server.sin_port = htons(PORT);
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

void SendDataToServer(Mat &image)
{
    IplImage *img1 = new IplImage(image);

    int bytes = 0;

//	if (_changeDisparityDynamically) {
//		tutorial::Packet inform;
//		inform.set_warnmsg(warnCode);
//		char* temp[10];
//		inform.SerializeToArray(temp,10);
//		send(clientsock, temp, 10, 0);
//	}

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
//    int n = 0;
//    fd_set input;
//    struct timeval timeout;
//
//    FD_ZERO(&input);
//    FD_SET(clientsock, &input);
//
//    timeout.tv_sec = 0;
//    timeout.tv_usec = 1;
//
//    n = select(clientsock + 1, &input, NULL, NULL, &timeout);
//    /* start sending images */
//    if (n < 0) {
//        perror("select failed");
//    } else if (n == 0) {
//
//    } else if (FD_ISSET(clientsock,&input)) {
//        char buffer[32];
//        recv(clientsock, buffer, sizeof(buffer), 0);
//        tutorial::Packet header;
//        header.ParseFromArray(buffer, sizeof(buffer));
//        if (header.has_channel()) {
//            switch (header.channel()) {
//            case Channel1:
//                currentChannel = Channel1;
//                break;
//            case Channel2:
//                currentChannel = Channel2;
//                break;
//            default:
//                break;
//            }
//
//        } else if(header.has_headertype()) {
//            switch (header.headertype()) {
//            case Normal:
//                currentMode = Normal;
//                break;
//            case Blob:
//                currentMode = Blob;
//                destroyAllWindows();
//                _displayBlobs = true;
//                break;
//            case Disparity:
//                currentMode = Disparity;
//                destroyAllWindows();
//                _displayBlobs = false;
//                break;
//            default:
//                break;
//            }
//        } else if(header.has_directionx()){
//            cout << "X coordinate is : "<< header.directionx() << endl;
//            cout << "Y coordinate is : "<< header.directiony() << endl;
//        }
//    }
}

void SendInstructions()
{
    int sockfd, n;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char mesg[10];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY );
    servaddr.sin_port = htons(9900);
    bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    for (;;)
    {
        len = sizeof(cliaddr);
        n = recvfrom(sockfd, mesg, 10, 0, (struct sockaddr *) &cliaddr, &len);
        int speed[13];
        speed[0] = 0x06;
        speed[1] = 0x00;
        speed[2] = 0x00;
        speed[3] = 0x00;
        speed[4] = 0x00;
        speed[5] = 0x00;
        speed[6] = 0x10;
        speed[7] = 0x20;
        speed[8] = 0x00;
        speed[9] = 0x00;
        speed[10] = 0x00;
        speed[11] = 0x00;
        speed[12] = 0x00;
        sendto(sockfd, speed, n, 0, (struct sockaddr *) &cliaddr,
                sizeof(cliaddr));
    }
}
