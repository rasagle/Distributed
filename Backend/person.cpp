#include "person.h"
using namespace std;

Person::Person(const string& username): username(username){
	userMut = new mutex;
	followedMut = new mutex;
	followerMut = new mutex;
	tweetMut = new mutex;
}

string Person::getUser() const{
	return username;
}

Person::~Person(){
	delete userMut;
	delete followedMut;
	delete followerMut;
	delete tweetMut;
}