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
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <cstdlib>

#define MAXLINE   4096  // max text line length
#define BUFFSIZE  8192    // buffer size for reads and writes
#define  SA struct sockaddr
#define LISTENQ   1024  // 2nd argument to listen()
#define PORT_NUM  13002
#define DELIMETER 178

void addUserFiles(const std::string& username);
std::string registerUser(const std::string& username, const std::string& password);
std::string loginUser(const std::string& username, const std::string& password);
void removeNameFromFile(const std::string& username, const std::string& filename);
void removeFollowerInfo(const std::string& username);
void removeAccount(const std::string& username);
void findEveryone(const std::string& username);
void findPeople(const std::string& currentUser, const std::string& username);
void getFollowNames(const std::string& username, int connfd);
void unfollow(const std::string& username, const std::string& name);
std::string checkUserExists(const std::string& username);
void displayTweets(const std::string& username, int connfd);
void tweet(const std::string& username, const std::string& timestamp, const std::string& message, int connfd);
std::string requestHandler(const std::string& message, int connfd);
	



















#endif