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
public:
    Car();
    void driveSafe(int direction, int speed, int objDistance, FLAGS::VISUALS);
    void driveUnsafe(int, int);
    void brake();
    double speed();
    void autoDrive();
    FLAGS::CARSTATUS status();
};


#endif /* CAR_H_ */
