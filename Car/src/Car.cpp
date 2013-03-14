#include "../header/Car.h"
#include "../header/Arduino.h"

#define PORT_inst 13400

//Arduino arduino("/dev/ttyACM0", 57600);
Car::Car()
{
    cout << "Creating car object." << endl;
    InitialiseConnection();
    InitialiseArray();

    carStatus = FLAGS::MOBILE;
}

void Car::InitialiseConnection()
{
    cout << "Opening port for car..";

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("192.168.0.20");
    servaddr.sin_port = htons(PORT_inst);

    cout << "\tPort open" << endl;
}

void Car::InitialiseArray()
{
    _dataPacket[0] = 0x06;
    for (int i = 1; i < 13; i++)
        _dataPacket[i] = 0x00;
}

void Car::driveSafe(uint8_t instructions[2], FLAGS::VISUALS visual)
{
    cout << instructions [0] << "\t" << instructions[1] << endl;
     if(visual == FLAGS::FAR)
        {
         _dataPacket[5] = instructions[0];
            _dataPacket[7] = instructions[1];

             sendto(sockfd, _dataPacket, sizeof(_dataPacket), 0,
                     (struct sockaddr *) &servaddr, sizeof(servaddr));

             carStatus = FLAGS::MOBILE;

        } else brake();
}

void Car::driveUnsafe(int direction, int speed)
{
    switch (speed)
    {
    case 0: //0
        _dataPacket[5] = 0x81;
        break;
    case 1: //1
        _dataPacket[5] = 0x84;
        break;
    case 2: //2
        _dataPacket[5] = 0x85;
        break;
    case 3: //3
        _dataPacket[5] = 0x86;
        break;
    case 4: //4
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
    case 1: //25
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

    carStatus = FLAGS::MOBILE;
}


void Car::brake()
{
    _dataPacket[5] = 0x00;
    _dataPacket[7] = 0x81;

    sendto(sockfd, _dataPacket, sizeof(_dataPacket), 0,
            (struct sockaddr *) &servaddr, sizeof(servaddr));

    carStatus = FLAGS::STATIONARY;
    cout << "Braked." << endl;
}

void Car::autoDrive()
{
    driveUnsafe(0,3);
    cout << "press key" << endl;
    string dum ;
    cin >> dum;
    brake();
}

FLAGS::CARSTATUS Car::status()
{
    return carStatus;
}
