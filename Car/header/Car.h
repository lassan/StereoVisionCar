/*
 * Car.h
 *
 *  Created on: Feb 27, 2013
 *      Author: cse
 */

#ifndef CAR_H_
#define CAR_H_

#include "includeFiles.h"

class Car
{
    void InitialiseConnection();
    void InitialiseArray();

    unsigned char _dataPacket[13];
    struct sockaddr_in servaddr;
    int sockfd;
    FLAGS::CARSTATUS carStatus;
    FLAGS::CARSTATUS prevStatus;

    bool brakeLightOn = false;

public:
    Car();

    void driveSafe(uint8_t instructions[2], FLAGS::VISUALS visual);
    void driveUnsafe(int, int);
    void brake();
    void autoDrive();
    string getSpeedString();
    double getSpeed();

    void turnBrakeLightOn();
    void turnBrakeLightOff();


    FLAGS::CARSTATUS status();
};


#endif /* CAR_H_ */
