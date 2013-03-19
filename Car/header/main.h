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
void DisparityCalculationWorker();

void GenerateSuperImposedImages();


#endif /* MAIN_H_ */
