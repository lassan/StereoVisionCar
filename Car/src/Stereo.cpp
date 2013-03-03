#include "../header/includeFiles.h"
#include "../header/Stereo.h"

Stereo::Stereo(int disp, int SADWindowSize)
{
    cout << "Creating Stereo object." << endl;
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
        closestObjectDisparity = *it;

        objectBoundingBox = boundingBoxes[it - meanPixelValues.begin()]; //copy variable to return (it will go out of scope otherwise)
        return &objectBoundingBox;
    }
    else
        return NULL;
}

int Stereo::distanceToObject(Mat& dispMap, Mat& Q)
{
//    cout << "Reprojecting" << endl;

    Mat output = Mat(dispMap.size(), CV_32FC3);

    Scalar transform = Q * Scalar(0, 0, 120, 1);

    cout << transform.val[0] << "\t" << transform.val[1] << "\t"
            << transform.val[2] << "\t" << transform.val[3] << endl;

//    reprojectImageTo3D(dispMap, output, Q);

//    for(int row = 0; row < dispMap.rows; row++)
//    {
    for (int col = 0; col < dispMap.cols; col++)
//        {
//            cout << (double)output.at<Vec3b>(row,col)[0]
//                 << "\t" << (double)output.at<Vec3b>(row,col)[1]
//                 << "\t" << (double)output.at<Vec3b>(row,col)[2]
//                 << endl;
//        }
//    }
        return 0;

}

/*Must call closestObject() before this*/
int Stereo::getClosestObjectDisparity()
{
    return closestObjectDisparity;
}
