/*
 * main.h
 *
 *  Created on: 19 Feb 2013
 *      Author: hassan
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "../header/includeFiles.h"

/*ENUM definitions*/
struct StereoPair {
    Mat leftImage, rightImage;
};

struct FLAGS {
    enum NUMDISPARITY {
        ABSOLUTE = 0x01, INCREMENT = 0x02, DECREMENT = 0x04
    };
};

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

void SendDataToServer(Mat& image);
void checkForData();

void InitStereoBM(int disp, FLAGS::NUMDISPARITY flag = FLAGS::ABSOLUTE,
        int _SADWindowSize = 21);


void PrepareStereoPairForDisparityCalculation(StereoPair &input);
void GenerateSuperImposedImages();

Mat CalculateDisparityBM(StereoPair const &images);
bool FindBlobs(Mat &bm);
void ChangeDisparityDynamically();
void ListenForClient();
void SendInstructions();

void ImageAcquisitionWorker();
void DisparityCalculationWorker();



#endif /* MAIN_H_ */
