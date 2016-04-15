#ifndef PERSON_H
#define PERSON_H
#include <mutex>
#include <string>

class Person{
	std::string username;
public:
	std::mutex *userMut;
	std::mutex *followedMut;
	std::mutex *followerMut;
	std::mutex *tweetMut;
	
	Person(const std::string& username);
	std::string getUser() const;
	~Person();
	
};













#endif

