#ifndef BACKENDMANAGER_H
#define BACKENDMANAGER_H

#include <stdio.h>       // perror, snprintf
#include <stdlib.h>      // exit
#include <unistd.h>      // close, write
#include <string.h>      // strlen
#include <strings.h>     // bzero
#include <time.h>        // time, ctime
#include <sys/socket.h>  // socket, AF_INET, SOCK_STREAM,
                         // bind, listen, accept
#include <netinet/in.h>  // servaddr, INADDR_ANY, htons
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <cstdlib>
#include <map>
#include <thread>
#include <mutex>
#include <unordered_map>
#include "person.h"
#include "replica.h"

#define MAXLINE   4096  // max text line length
#define BUFFSIZE  8192    // buffer size for reads and writes
#define  SA struct sockaddr
#define LISTENQ   1024  // 2nd argument to listen()
#define PORT_NUM  13002
#define DELIMETER 178

class Manager{
private:
	std::mutex userBaseMut;
	std::mutex mapMut;
	std::unordered_map<std::string, Person*> uMap;
	std::vector<Replica> replicaVec;
	int portNum;
	bool status;
	std::string stuff;
	
public:
	Manager(const std::string& port, const std::string& stat);
	void addUserFiles(const std::string& username);
	std::string registerUser(const std::string& username, const std::string& password);
	std::string loginUser(const std::string& username, const std::string& password);
	void removeNameFromFile(const std::string& username, const std::string& filename);
	void removeFollowerInfo(const std::string& username);
	void removeAccount(const std::string& username);
	void findEveryone(const std::string& username, int connfd);
	void findPeople(const std::string& currentUser, const std::string& username);
	void getFollowNames(const std::string& username, int connfd);
	void unfollow(const std::string& username, const std::string& name);
	std::string checkUserExists(const std::string& username);
	void parseMessage(const std::string& username, std::vector<std::string>& vec);
	void displayTweets(const std::string& username, int connfd);
	void tweet(const std::string& username, const std::string& date, const std::string& time1, const std::string& message, int connfd);
	std::string numFollowers(const std::string& username);
	void parseMessage2(const std::string& username, std::map<std::string, std::string>& myMap);
	void aggregateFeed(const std::string& username, int connfd);
	void connectServ(int connfd, const std::string& ipaddr);
	bool sendAsClient(const std::string& request, const std::string& ipaddr, int port);
	void sendToReplica(const std::string& request);
	void changeIsUp(const std::string& request);
	void changeStatus(const std::string& request);
	std::string requestHandler(const std::string& message, int connfd, const std::string& ipaddr);
};


















#endif