/*
 * Car.h
 *
 *  Created on: Feb 27, 2013
 *      Author: cse
 */

#ifndef CAR_H_
#define CAR_H_

#include "../header/includeFiles.h"

class Car
{
	void InitialiseConnection();
	void InitialiseArray();
	void setStatus();

	unsigned char _dataPacket[13];
	struct sockaddr_in servaddr;
	int sockfd;
	FLAGS::CARSTATUS carStatus;
public:
	Car();
	void drive(int x, int y);
	void brake();
	int speed();
	FLAGS::CARSTATUS status();
};


#endif /* CAR_H_ */
