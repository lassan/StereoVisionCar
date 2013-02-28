#include "../header/Car.h"
#include "../header/Arduino.h"

#define PORT_inst 13400

Car::Car() {
	InitialiseConnection();
	InitialiseArray();
}

void Car::InitialiseConnection() {
	cout << "Opening port for car..";

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("192.168.0.20");
	servaddr.sin_port = htons(PORT_inst);

	cout << "\tPort open" << endl;
}

void Car::InitialiseArray() {
	_dataPacket[0] = 0x06;
	for (int i = 1; i < 13; i++)
		_dataPacket[i] = 0x00;
}

void Car::drive(int direction, int speed) {
	switch (speed) {
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

	switch (direction) {
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
		_dataPacket[8] = 0x00;
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

void Car::brake(){
	_dataPacket[5] = 0x00;
	_dataPacket[7] = 0x7d;

	int sentbyte = sendto(sockfd, _dataPacket, sizeof(_dataPacket), 0, (struct sockaddr *) &servaddr,
			sizeof(servaddr));
	if(sentbyte != -1) cout << "Car stopped." << endl;
}

int Car::speed(){

	return 0;
}

FLAGS::CARSTATUS Car::status()
{
	return carStatus;
}
