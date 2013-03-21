/*
 * main.h
 *
 *  Created on: 19 Feb 2013
 *      Author: hassan
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "../header/includeFiles.h"
#include "../header/main.h"
#include "../header/helperfunctions.h"
#include "../header/Server.h"
#include "../header/Stereo.h"
#include "../header/Car.h"
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <stdlib.h>

/*Function prototype declarations*/
void Initialise();
void InitCalibrationData();
void InitCameras();
void InitBuffers();

void GetStereoImages(StereoPair &input);
void ImageAcquisitionWorker();
void controlCar();
void DisparityCalculationWorker();

void GenerateSuperImposedImages();


Mat _M1, _D1, _M2, _D2, _R1, _R2, _P1, _P2, _Q;

VideoCapture _leftCamera, _rightCamera;
Mat _leftCameraMap1, _leftCameraMap2, _rightCameraMap1, _rightCameraMap2;

/*Variables for timing*/
int MAX_TIMING_ITERATIONS = 5;

#define OMPTHREADS 2

/*Variables for storing images*/
StereoPair _buffer0;
StereoPair _buffer1;
StereoPair _disparityBuffer;

#endif /* MAIN_H_ */
