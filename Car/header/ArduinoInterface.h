/*
 * ArduinoInterface.h
 *
 *  Created on: 31 Jan 2013
 *      Author: hassan
 */

#ifndef ARDUINOINTERFACE_H_
#define ARDUINOINTERFACE_H_

#include "../header/includeFiles.h"

class ArduinoInterface
{
    int Initialise(const char* serialport, int baud);
    int fda;
public:
    ArduinoInterface(const char*, int);
    int serialport_writebyte(uint8_t);
    int serialport_write(const char*);
    int serialport_read(char*, char);
};


#endif /* ARDUINOINTERFACE_H_ */
