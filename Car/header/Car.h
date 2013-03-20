/*
 * Car.h
 *
 *  Created on: Feb 27, 2013
 *      Author: cse
 */

#ifndef CAR_H_
#define CAR_H_

#include "includeFiles.h"
#include "../header/Arduino.h"

class Car
{
public:
    Car();
    void driveUnsafe(int, int);
    void brake();

    string getSpeedString();
    double getSpeed();

    void turnBrakeLightOn();
    void turnBrakeLightOff();
    void turnLeftIndicatorOn();
    void turnLeftIndicatorOff();
    void turnRightIndicatorOn();
    void turnRightIndicatorOff();

private:
    void initialiseConnection();
    void initialiseArray();

//    Arduino arduino;
    unsigned char _dataPacket[13];
    struct sockaddr_in servaddr;
    int sockfd;
    bool brakeLightOn, leftIndicatorOn, rightIndicatorOn;

    struct ARDUINOCMD
    {
        enum
        {
            GET_SPEED = 0x00,
            BRAKE_ON = 0x01,
            BRAKE_OFF = 0x02,
            LEFT_ON = 0x03,
            LEFT_OFF = 0x04,
            RIGHT_ON = 0x05,
            RIGHT_OFF = 0x06
        };
    };

};

#endif /* CAR_H_ */
