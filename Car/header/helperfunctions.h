/*
 * helperfunctions.h
 *
 *  Created on: 19 Feb 2013
 *      Author: hassan
 */

#ifndef HELPERFUNCTIONS_H_
#define HELPERFUNCTIONS_H_

#include "../header/includeFiles.h"
#include "../header/main.h"


Scalar GetBlobColor(float mean);
int LoadMatrixFromFile(string _filePath, string filename, Mat& mat);
string intToString(int num);
Mat OverlayImages(StereoPair, float scale);


#endif /* HELPERFUNCTIONS_H_ */
