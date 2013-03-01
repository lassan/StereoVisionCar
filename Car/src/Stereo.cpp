#include "../header/includeFiles.h"
#include "../header/Stereo.h"

Stereo::Stereo(int disp, int SADWindowSize)
{
    Initialise(disp, SADWindowSize);
}

void Stereo::Initialise(int disp, int SADWindowSize)
{
    sbm = StereoBM(StereoBM::BASIC_PRESET, 16 * disp, SADWindowSize);
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
