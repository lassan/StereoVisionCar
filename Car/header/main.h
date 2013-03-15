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


/*Function prototype declarations*/
void Initialise();
void InitCalibrationData();
void InitCameras();

void GetStereoImages(StereoPair &input);
void GenerateSuperImposedImages();

void ChangeDisparityDynamically();

void ImageAcquisitionWorker();
void DisparityCalculationWorker();



#endif /* MAIN_H_ */
