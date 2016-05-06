#include "replica.h"

Replica::Replica(int portNum, bool status, bool isUp) :portNum(portNum), status(status), isUp(isUp){}

int Replica::getPort() const{
	return portNum;
}

bool Replica::getStatus() const{
	return status;
}

void Replica::changeStatus(bool newStatus){
	status = newStatus;
}

bool Replica::getIsUp()const{
	return isUp;
}

void Replica::changeIsUp(bool newIsUp){
	isUp = newIsUp;
}