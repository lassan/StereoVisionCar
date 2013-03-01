/*
 * ArduinoInterface.h
 *
 *  Created on: 31 Jan 2013
 *      Author: hassan
 */

#ifndef ARDUINO_H_
#define ARDUINO_H_

#include "../header/includeFiles.h"

class Arduino
{
    int Initialise(const char* serialport, int baud);
    int fda;
public:
    Arduino(const char*, int);
    int serialport_writebyte(uint8_t);
    int serialport_write(const char*);
    int serialport_read(char*);
};


#endif /* ARDUINO_H_ */
