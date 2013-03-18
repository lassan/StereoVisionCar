/*
 * Server.h
 *
 *  Created on: 15 Mar 2013
 *      Author: hassan
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "../header/includeFiles.h"
#include "../header/Car.h"

#define PORT_send 8800

class Server
{
public:

    void initialise();
    void sendData(StereoPair &input, Mat &image, bool shouldBrake, Car &car, string message);

private:
    void displayImage(StereoPair &input, Mat &image);
    void sendImageToClient(Mat& image);
    void sendBrakeData(bool shouldBrake);
    void sendMessage(string message);
    void sendSpeed(string speed);
    void checkForData();
    void listenForClient();

    int serversock, clientsock;
    int hasClient;
    FLAGS::CLIENTDISPLAY streamDisplayType;
};

#endif /* SERVER_H_ */
