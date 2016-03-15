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
#include <cstdlib>

#define MAXLINE   4096  // max text line length
#define BUFFSIZE  8192    // buffer size for reads and writes
#define  SA struct sockaddr
#define LISTENQ   1024  // 2nd argument to listen()
#define PORT_NUM  13002
#define DELIMETER 178

std::string registerUser(std::string& username, std::string& password);
std::string requestHandler(std::string& message);
	



















#endif