/*
 * helperfunctions.cpp
 *
 *  Created on: 19 Feb 2013
 *      Author: hassan
 */

#include "../header/helperfunctions.h"

Scalar GetBlobColor(float mean) {
    int roundedMean = round((float) mean / 16.0) * 16;

    switch (roundedMean) {
    case 16 * 1:
        return CV_RGB(0,0,107);

    case 16 * 2:
        return CV_RGB(0,79,158);

    case 16 * 3:
        return CV_RGB(0,105,209);

    case 16 * 4:
        return CV_RGB(5,130,255);

    case 16 * 5:
        return CV_RGB(31,143,255);

    case 16 * 6:
        return CV_RGB(92,173,255);

    case 16 * 7:
        return CV_RGB(92,255,255);

    case 16 * 8:
        return CV_RGB(92,255,173);

    case 16 * 9:
        return CV_RGB(92,255,92);

    case 16 * 10:
        return CV_RGB(0,224,0);

    case 16 * 11:
        return CV_RGB(0,163,0);

    case 16 * 12:
        return CV_RGB(0,102,0);

    case 16 * 13:
        return CV_RGB(255,153,0);

    case 16 * 14:
        return CV_RGB(255,51,51);

    case 16 * 15:
        return CV_RGB(255,0,0);

    case 16 * 16:
        return CV_RGB(255,255,255);

    default:
        return CV_RGB(0,0,0);
    }

}

int LoadMatrixFromFile(string _filePath, string filename, Mat& mat) {
    string filePath = _filePath + filename + ".xml";
    FileStorage fs(filePath, FileStorage::READ);
    if (fs.isOpened()) {
        fs[filename] >> mat;
        cout << filename << " loaded" << endl;
        fs.release();
        return 0;
    } else {
        cout << filePath + " could not be opened" << endl;
        return -1;
    }
}



string intToString(int num) {
    ostringstream myStream;
    myStream << num << flush;
    return (myStream.str());
}

Mat OverlayImages(StereoPair camImages, float scale) {
    Mat overlay = Mat(camImages.leftImage.size(), camImages.leftImage.type());

    uint8_t* l = (uint8_t*) camImages.leftImage.data;
    uint8_t* r = (uint8_t*) camImages.rightImage.data;
    uint8_t* n = (uint8_t*) overlay.data;

    int cn = camImages.leftImage.channels();
    int cols = camImages.leftImage.cols;

    Scalar_<uint8_t> lPixel;
    Scalar_<uint8_t> rPixel;

    for (int i = 0; i < camImages.leftImage.rows; i++) {
        for (int j = 0; j < camImages.leftImage.cols; j += cn) {


            lPixel.val[0] = l[i * cols * cn + j * cn + 0]; // B
            lPixel.val[1] = l[i * cols * cn + j * cn + 1]; // G
            lPixel.val[2] = l[i * cols * cn + j * cn + 2]; // R

            rPixel.val[0] = r[i * cols * cn + j * cn + 0]; // B
            rPixel.val[1] = r[i * cols * cn + j * cn + 1]; // G
            rPixel.val[2] = r[i * cols * cn + j * cn + 2]; // R

            n[i * cols * cn + j * cn + 0] = ((int) lPixel.val[0] * scale)
                    + ((int) rPixel.val[0] * scale);
            n[i * cols * cn + j * cn + 1] = ((int) lPixel.val[1] * scale)
                    + ((int) rPixel.val[1] * scale);
            n[i * cols * cn + j * cn + 2] = ((int) lPixel.val[2] * scale)
                    + ((int) rPixel.val[2] * scale);

        }
    }
    return overlay;
}
