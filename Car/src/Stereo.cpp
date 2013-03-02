#include "../header/includeFiles.h"
#include "../header/Stereo.h"

Stereo::Stereo(int disp, int SADWindowSize)
{
    Initialise(disp, SADWindowSize);
}

void Stereo::Initialise(int disp, int SADWindowSize)
{
    nDisparity = 16 * disp;
    sbm = StereoBM(StereoBM::BASIC_PRESET, nDisparity, SADWindowSize);
}

//If flag is ASOLUTE, set nDisparity to 16 * disp, else increment/decrement
//current disparity by 16 * disp
void Stereo::changeParameters(int disp, FLAGS::NUMDISPARITY flag,
        int _SADWindowSize)
{
    if (flag == FLAGS::INCREMENT)
        disp = (nDisparity / 16) + disp;
    else if (flag == FLAGS::DECREMENT)
        disp = (nDisparity / 16) - disp;

    if (disp < 1)
        throw "Disparity can not be set to zero.";
    Initialise(disp, _SADWindowSize);
}

Mat Stereo::disparityMap(StereoPair &images)
{
    int rows = images.leftImage.rows;
    int cols = images.leftImage.cols;

    Mat imgDisparity16S = Mat(rows, cols, CV_16S);
    Mat imgDisparity8U = Mat(rows, cols, CV_8UC1);

//Calculate the disparity image
    sbm(images.leftImage, images.rightImage, imgDisparity16S, CV_16S);

//Check the extreme values
    double minVal;
    double maxVal;
    minMaxLoc(imgDisparity16S, &minVal, &maxVal);

//Prepare for display
    imgDisparity16S.convertTo(imgDisparity8U, CV_8UC1, 255 / (maxVal - minVal));

    return imgDisparity8U;
}

Rect* Stereo::closestObject(Mat &dispMap)
{
    Mat element(3, 3, CV_8U, cv::Scalar(1));

    erode(dispMap, dispMap, element);

    IplImage *dispIpl = new IplImage(dispMap); //create an IplImage from Mat

    //Declare variables
    CBlobResult blobs;
    int minArea = 125;

    blobs = CBlobResult(dispIpl, NULL, 0); //get all blobs in the disparity map
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, minArea); //filter blobs by area and remove all less than minArea

    CBlob* currentBlob;
    vector<int> meanPixelValues;
    vector<Rect> boundingBoxes;

    for (int i = 0; i < blobs.GetNumBlobs(); i++)
    {
        currentBlob = blobs.GetBlob(i);
        meanPixelValues.push_back(currentBlob->Mean(dispIpl));
        boundingBoxes.push_back(currentBlob->GetBoundingBox());
    }

    vector<int>::const_iterator it;

    if (meanPixelValues.size() > 0)
    {
        it = max_element(meanPixelValues.begin(), meanPixelValues.end());
        return &boundingBoxes[it - meanPixelValues.begin()];
    }
    else
        return NULL;
}

