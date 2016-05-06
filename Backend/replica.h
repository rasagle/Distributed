#ifndef REPLICA_H
#define REPLICA_H
class Replica{
private:
	int portNum;
	bool status;
	bool isUp;
public:
	Replica(int portNum, bool status, bool isUp);
	int getPort() const;
	bool getStatus() const;
	void changeStatus(bool newStatus);
	bool getIsUp() const;
	void changeIsUp(bool newIsUp);
};


#endif