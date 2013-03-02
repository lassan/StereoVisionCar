/*
 * main.h
 *
 *  Created on: 19 Feb 2013
 *      Author: hassan
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "../header/includeFiles.h"


/*Function prototype declarations*/
void Initialise();
void InitCalibrationData();
void InitCameras();
void InitServer();
void InitDirectionsFromClient();
void InitCarConnection();
void InitDrivingArray();

void CarDrivingWorker();
void SendInstructionsToCar(int dir_x, int dir_y);
void StopTheCar();

void SendDataToClient(Mat& image);
void checkForData();

void ClientDisplay(StereoPair &input, Mat &image);


void GetStereoImages(StereoPair &input);
void GenerateSuperImposedImages();

Mat CalculateDisparityBM(StereoPair &images);
bool FindBlobs(Mat &bm);
void ChangeDisparityDynamically();
void ListenForClient();
void SendInstructions();

void ImageAcquisitionWorker();
void DisparityCalculationWorker();



#endif /* MAIN_H_ */
