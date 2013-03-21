#include "../header/Car.h"

#define PORT_inst 13400

Arduino arduino("/dev/ttyACM0", 19200);
Car::Car()
{
    cout << "Creating car object." << endl;
    initialiseConnection();
    initialiseArray();

//   try{
//        arduino.initialise("/dev/ttyACMO", 56700);
//    }catch(...){
//        cerr << "Connecting to the arduino failed." << endl;
//    }

    brakeLightOn = false;
    leftIndicatorOn = false;
    rightIndicatorOn = false;
}

void Car::initialiseConnection()
{
    cout << "Opening port for car..";

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("192.168.0.20");
    servaddr.sin_port = htons(PORT_inst);

    cout << "\tPort open" << endl;
}

void Car::initialiseArray()
{
    _dataPacket[0] = 0x06;
    for (int i = 1; i < 13; i++)
        _dataPacket[i] = 0x00;
}

void Car::driveUnsafe(int direction, int speed)
{
    switch (speed)
    {
        case 0: //0
            _dataPacket[5] = 0x81;
            break;
        case 1: //3
            _dataPacket[5] = 0x84;
            break;
        case 2: //4
            _dataPacket[5] = 0x85;
            break;
        case 3: //5
            _dataPacket[5] = 0x86;
            break;
        case 4: //6
            _dataPacket[5] = 0x87;
            break;
        case -1: //-1
            _dataPacket[5] = 0x7e;
            break;
        case -2: //-2
            _dataPacket[5] = 0x7d;
            break;
        case -3: //-3
            _dataPacket[5] = 0x7c;
            break;
        case -4: //-4
            _dataPacket[5] = 0x7a;
            break;
        default:
            brake();
            break;
    }

    switch (direction)
    {
        case 0:
            _dataPacket[7] = 0x81;
            break;
        case 1: //25speed
            _dataPacket[7] = 0xa0;
            break;
        case 2: //50
            _dataPacket[7] = 0xc0;
            break;
        case 3: //75
            _dataPacket[7] = 0xe0;
            break;
        case 4: //99
            _dataPacket[7] = 0xff;
            break;
        case -1: //-25
            _dataPacket[7] = 0x5f;
            break;
        case -2: //-50
            _dataPacket[7] = 0x3f;
            break;
        case -3: //-75
            _dataPacket[7] = 0x1f;
            break;
        case -4: //-100
            _dataPacket[7] = 0x00;
            break;
        default:
            cout << "Default case" << endl;
            brake();
            break;
    }
    sendto(sockfd, _dataPacket, sizeof(_dataPacket), 0,
            (struct sockaddr *) &servaddr, sizeof(servaddr));
}

void Car::brake()
{
    _dataPacket[5] = 0x00;
    _dataPacket[7] = 0x81;

    sendto(sockfd, _dataPacket, sizeof(_dataPacket), 0,
            (struct sockaddr *) &servaddr, sizeof(servaddr));

    turnBrakeLightOn();
//    cout << "Braked." << endl;
}

//returns speed as a string
string Car::getSpeedString()
{
    arduino.serialport_writebyte(0x00);
    try
    {
        return arduino.serialport_read();
    } catch (...)
    {
        return "-1";
    }
    return "-1";
}

double Car::getSpeed()
{
    arduino.serialport_writebyte(0x00);
    try
    {
        return atof(arduino.serialport_read().c_str());
    } catch (...)
    {
        return -1;
    }
    return -1;
}

void Car::turnBrakeLightOn()
{
    if (!brakeLightOn)
    {
        arduino.serialport_writebyte(ARDUINOCMD::BRAKE_ON);
        brakeLightOn = true;
    }
}

void Car::turnBrakeLightOff()
{
    if (brakeLightOn)
    {
        arduino.serialport_writebyte(ARDUINOCMD::BRAKE_OFF);
        brakeLightOn = false;
    }
}

void Car::turnLeftIndicatorOn()
{
    if (!leftIndicatorOn)
    {
        arduino.serialport_writebyte(ARDUINOCMD::LEFT_ON);
        leftIndicatorOn = true;
    }
}

void Car::turnLeftIndicatorOff()
{
    if (leftIndicatorOn)
    {
        arduino.serialport_writebyte(ARDUINOCMD::LEFT_OFF);
        leftIndicatorOn = false;
    }
}

void Car::turnRightIndicatorOn()
{
    if (!rightIndicatorOn)
    {
        arduino.serialport_writebyte(ARDUINOCMD::RIGHT_ON);
        rightIndicatorOn = true;
    }
}

void Car::turnRightIndicatorOff()
{
    if (rightIndicatorOn)
    {
        arduino.serialport_writebyte(ARDUINOCMD::RIGHT_OFF);
        rightIndicatorOn = false;
    }
}
