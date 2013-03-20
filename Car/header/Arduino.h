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
private:
    int fd;
public:
    Arduino(const char* serialport, int baud);
    void initialise(const char* serialport, int baud);
    int serialport_writebyte(uint8_t);
    int serialport_write(const char*);
    string serialport_read();
};


#endif /* ARDUINO_H_ */
