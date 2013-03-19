#include "../header/includeFiles.h"
#include "../header/Stereo.h"
#include "../header/Car.h"

#define DISPCHANGEMAX 190
#define DISPCHANGEMIN 140

Stereo::Stereo(int SADWindowSize)
{
    cout << "Creating Stereo object." << endl;
    maxDisp = 5;
    minDisp = 3;
    numObjects = 3;
    visual = FLAGS::FAR;

    Initialise(minDisp, SADWindowSize);
}

void Stereo::Initialise(int disp, int SADWindowSize)
{
    nDisparity = 16 * disp;
    sbm = StereoBM(StereoBM::BASIC_PRESET, nDisparity, SADWindowSize);
}

bool Stereo::changeParameters(int _SADWindowSize, Car &car)
{
    if (dispChange == FLAGS::INCREMENT)
    {
        Initialise(maxDisp, _SADWindowSize);
        visual = FLAGS::NEAR;
        return true;
    }
    else if (dispChange == FLAGS::DECREMENT)
    {
        Initialise(minDisp, _SADWindowSize);
        visual = FLAGS::FAR;
        return true;
    }
    else
        return false;
}

FLAGS::VISUALS Stereo::getVisualInfo()
{
    return visual;
}

//To decide whether the disparity buffer needs to be discarded
bool Stereo::parameterChangeRequired()
{
    if (dispChange == FLAGS::INCREMENT || dispChange == FLAGS::DECREMENT)
        return true;
    else
        return false;
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

bool Stereo::detectObjects(Mat &dispMap)
{

    blur(dispMap, dispMap, Size(3,3));

    Mat element(3, 3, CV_8U, cv::Scalar(1));

    erode(dispMap, dispMap, element);

//    imshow("dispMap", dispMap);
//    waitKey(5);

    IplImage *dispIpl = new IplImage(dispMap); //create an IplImage from Mat

    //Declare variables
    CBlobResult blobs;
    int minArea = 250;

    blobs = CBlobResult(dispIpl, NULL, 0); //get all blobs in the disparity map
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, minArea); //filter blobs by area and remove all less than minArea

    numObjects = blobs.GetNumBlobs();

    CBlob* currentBlob;
    vector<int> meanPixelValues;
    vector<Rect> boundingBoxes;

    for (int i = 0; i < numObjects; i++)
    {
        currentBlob = blobs.GetBlob(i);
        meanPixelValues.push_back(currentBlob->Mean(dispIpl));
        boundingBoxes.push_back(currentBlob->GetBoundingBox());
    }

    vector<int>::const_iterator it;

    if (meanPixelValues.size() > 0)
    {
        it = max_element(meanPixelValues.begin(), meanPixelValues.end());
        closestObjectVal = *it;
        objArea = currentBlob->Area();
        objectBoundingBox = boundingBoxes[it - meanPixelValues.begin()]; //copy variable to return (it will go out of scope otherwise)

        if (closestObjectVal > DISPCHANGEMAX)
            dispChange = FLAGS::INCREMENT;
        else if (closestObjectVal < DISPCHANGEMIN)
            dispChange = FLAGS::DECREMENT;
        else
            dispChange = FLAGS::ABSOLUTE;

        return true;
    }
    else
    {
        closestObjectVal = -1;
        objArea = -1;
        return false;
    }
}

int Stereo::getNumObjects()
{
    return numObjects;
}

/* Must call closestObject() before this *
 * Returns the average pixel value of object in disparity map*/
int Stereo::getClosestObjectVal()
{
    return closestObjectVal;
}

double Stereo::getClosestObjectArea()
{
    return objArea;
}

//checks if less than 2 objects are present
// but area is very large, could mean further away landscape, so don't stop
bool Stereo::texturelessObjectPresent()
{
    if (numObjects <= 1 && objArea < 10000)
    {
        cout << "Textureless Object" << endl;

        return true;
    }
    else
        return false;
}

bool Stereo::shouldBrake()
{
//  if (visual == FLAGS::NEAR || texturelessObjectPresent())
  if(visual == FLAGS::NEAR)
    {
        return true;
    }
    else
        return false;
}

