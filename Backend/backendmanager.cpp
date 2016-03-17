#include "backendmanager.h"
using namespace std;

void addUserFiles(const string& username){
	ofstream ofs( username + "_messages.txt" );
	ofstream ofs1( username + "_followers.txt" );
	ofstream ofs2( username + "_followed.txt" );
	ofs.close();
	ofs1.close();
	ofs2.close();
}

string registerUser(const string& username, const string& password){
	
	ifstream ifs( username + ".txt" );
	if(!ifs){
		ofstream ofs( username + ".txt" );
		ofs << username + " " + password;
		ofs.close();
		ofstream ofs1( "userbase.txt", ofstream::app );
		ofs1 << username + "\n";
		ofs1.close();
		addUserFiles(username);
		return("Register successful");
	}
	else{
		return("Username already exists");
	}
}

string loginUser(const string& username, const string& password){
	
	ifstream ifs( username + ".txt" );
	if(!ifs)
		return ("The user is not registered");
	else{
		string user, pass;
		ifs >> user >> pass;
		if( user == username && pass == password )
			return ("Login successful");
		else
			return ("Invalid credentials");
	}
}

void removeNameFromFile(const string& username, const string& filename){
	ifstream ifs(filename);
	ofstream ofs("temp.txt");
	string line;
	while(getline(ifs,line)){
		if(line != username)
			ofs << line + '\n';
	}
	ifs.close();
	ofs.close();
	remove(filename.c_str());
	rename("temp.txt", filename.c_str());
}

void removeFollowerInfo(const string& username){
	ifstream ifs(username + "_followed.txt");
	string followedName;
	while(getline(ifs, followedName)){
		removeNameFromFile(username, followedName + "_followers.txt");
	}
	ifs.close();
	
	ifstream ifs1(username + "_followed.txt");
	string followerName;
	while(getline(ifs1, followerName)){
		removeNameFromFile(username, followerName + "_followed.txt");
	}
	ifs1.close();
}

void removeAccount(const string& username){
	removeFollowerInfo(username);
	remove((username + ".txt").c_str());
	remove((username + "_followed.txt").c_str());
	remove((username + "_followers.txt").c_str());
	remove((username + "_messages.txt").c_str());
	removeNameFromFile(username, "userbase.txt");
}

void findEveryone(const string& username, int connfd){
	ifstream ifs("userbase.txt");
	string line;
	set<string> mySet;
	set<string>::iterator it;
	while(getline(ifs, line)){
		if(line != username)
			mySet.insert(line);
	}
	ifs.close();
	//removes everyone that the user already followed
	ifstream ifs1(username + "_followed.txt");
	string newLine;
	while(getline(ifs1, newLine)){
		mySet.erase(newLine);
	}
	ifs1.close();
	for(it = mySet.begin(); it != mySet.end(); ++it){
		string line = *it + "*~";
		send(connfd, line.c_str(), strlen(line.c_str()), 0);
	}
	//string stopMsg = "No more people";
	//send(connfd, stopMsg.c_str(), strlen(stopMsg.c_str()), 0);
}

void findPeople(const string& currentUser, const string& username){
	ofstream ofs(currentUser + "_followed.txt", ofstream::app);
	ofs << username + '\n';
	ofs.close();
	ofstream ofs1(username + "_followers.txt", ofstream::app);
	ofs1 << currentUser + '\n';
	ofs1.close();
}

void getFollowNames(const string& username, int connfd){
	ifstream ifs(username + "_followed.txt");
	string line;
	set<string> mySet;
	set<string>::iterator it;
	while(getline(ifs, line)){
		mySet.insert(line);
	}
	ifs.close();
	for(it = mySet.begin(); it != mySet.end(); ++it){
		string newLine = *it + "~*~";
		send(connfd, newLine.c_str(), strlen(newLine.c_str()), 0);
	}
}

void unfollow(const string& username, const string& name){
	removeNameFromFile(name, username + "_followed.txt");
	removeNameFromFile(username, name + "_followers.txt");
}

string checkUserExists(const string& username){
	ifstream ifs(username + "_messages.txt");
	if(!ifs)
		return("Does not exist");
	ifs.close();
	return("Exists");
}

void displayTweets(const string& username, int connfd){
	ifstream ifs(username + "_messages.txt");
	ostringstream ss;
	ss << ifs.rdbuf();
	string s = ss.str();
	ifs.close();
	vector<string> vec;
	char str[s.size()+1];
	strcpy(str, s.c_str());
	char * pch;
	pch = strtok(str, "~*~");
	while(pch != nullptr){
		vec.push_back(pch);
		pch = strtok(nullptr, "~*~");
	}
	for(int i = 0; i < vec.size(); ++i){
		send(connfd, vec[i].c_str(), strlen(vec[i].c_str()), 0);
	}
}

void tweet(const string& username, const string& timestamp, const string& message, int connfd){
	ofstream ofs(username + "_messages.txt", ofstream::app);
	ofs << timestamp + " -- " + message + "~*~\n";
	ofs.close();
	displayTweets(username, connfd);
}

string requestHandler(const string& message, int connfd){
	
	string request;
	istringstream iss(message);
	iss >> request;
	
	if( request == "register" ){
		string info;
		iss >> info;
		int index = info.find('*');
		string user = info.substr(0, index);
		string pass = info.substr(index+1, info.size());
		return registerUser(user, pass);
	}
	else if( request == "login" ){
		string info;
		iss >> info;
		int index = info.find('*');
		string user = info.substr(0, index);
		string pass = info.substr(index+1, info.size());
		return loginUser(user, pass);
	}
	else if( request == "remove_account" ){
		string user;
		iss >> user;
		removeAccount(user);
		return("Account removed");
	}
	else if( request == "find_everyone" ){
		string user;
		iss >> user;
		findEveryone(user, connfd);
		return("Everyone found");
	}
	else if( request == "find_people" ){
		string currentUser, user;
		iss >> currentUser >> user;
		findPeople(currentUser, user);
		return("Succesfully added");
	}
	else if( request == "get_unfollow_name" ){
		string user;
		iss >> user;
		getFollowNames(user, connfd);
		return("Unfollowers name");
	}
	else if( request == "unfollow" ){
		string username, name;
		iss >> username >> name;
		unfollow(username, name);
		return("Unfollowed");
	}
	else if( request == "view_followed" ){
		string user;
		iss >> user;
		getFollowNames(user, connfd);
		return("Followed names");
	}
	else if( request == "check_user_exists" ){
		string user;
		iss >> user;
		return checkUserExists(user);
	}
	else if( request == "display_tweets" ){
		string user;
		iss >> user;
		displayTweets(user, connfd);
		return("Displayed Tweets");
	}
	else if( request == "tweet" ){
		string user, timestamp, message;
		iss >> user >> timestamp >> message;
		tweet(user, timestamp, message, connfd);
		return("Tweeted");
	}
}