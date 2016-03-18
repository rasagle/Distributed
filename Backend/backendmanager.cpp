#include "backendmanager.h"
using namespace std;

void addUserFiles(const string& username){
	//Creates 4 user files
	ofstream ofs( username + "_messages.txt" );
	ofstream ofs1( username + "_followers.txt" );
	ofstream ofs2( username + "_followed.txt" );
	ofs.close();
	ofs1.close();
	ofs2.close();
}

string registerUser(const string& username, const string& password){
	//Registers the user into the system
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
	//Logs the user into the system
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
	//Removes an instance of the word from a file
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
	//Goes into followers/followed files and removes the username
	ifstream ifs(username + "_followed.txt");
	string followedName;
	//Goes into the follower's file and removes username
	while(getline(ifs, followedName)){
		removeNameFromFile(username, followedName + "_followers.txt");
	}
	ifs.close();
	//Goes into the followed file and removes username
	ifstream ifs1(username + "_followed.txt");
	string followerName;
	while(getline(ifs1, followerName)){
		removeNameFromFile(username, followerName + "_followed.txt");
	}
	ifs1.close();
}

void removeAccount(const string& username){
	//Removes all the user files and info from respective files
	removeFollowerInfo(username);
	remove((username + ".txt").c_str());
	remove((username + "_followed.txt").c_str());
	remove((username + "_followers.txt").c_str());
	remove((username + "_messages.txt").c_str());
	removeNameFromFile(username, "userbase.txt");
}

void findEveryone(const string& username, int connfd){
	//Finds everyone that the user 
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
	//Sends the information to socket
	for(it = mySet.begin(); it != mySet.end(); ++it){
		string line = *it + "~*~";
		send(connfd, line.c_str(), strlen(line.c_str()), 0);
	}
}

void findPeople(const string& currentUser, const string& username){
	//Adds people to corresponding files
	ofstream ofs(currentUser + "_followed.txt", ofstream::app);
	ofs << username + '\n';
	ofs.close();
	ofstream ofs1(username + "_followers.txt", ofstream::app);
	ofs1 << currentUser + '\n';
	ofs1.close();
}

void getFollowNames(const string& username, int connfd){
	//Sends name of people that the user have followed
	ifstream ifs(username + "_followed.txt");
	string line;
	set<string> mySet;
	set<string>::iterator it;
	while(getline(ifs, line)){
		mySet.insert(line);
	}
	ifs.close();
	//Sends information to socket
	for(it = mySet.begin(); it != mySet.end(); ++it){
		string newLine = *it + "~*~";
		send(connfd, newLine.c_str(), strlen(newLine.c_str()), 0);
	}
}

void unfollow(const string& username, const string& name){
	//Unfollow one user, removes name from corresponding file
	removeNameFromFile(name, username + "_followed.txt");
	removeNameFromFile(username, name + "_followers.txt");
}

string checkUserExists(const string& username){
	//Checks to see if a user exists
	ifstream ifs(username + "_messages.txt");
	if(!ifs)
		return("Does not exist");
	ifs.close();
	return("Exists");
}

void parseMessage(const string& username, vector<string>& vec){
	//Parses user's message into separate strings and stores in a vector
	ifstream ifs(username + "_messages.txt");
	string line;
	while(getline(ifs,line)){
		vec.push_back(line);
	}
	ifs.close();
	/*
	ostringstream ss;
	ss << ifs.rdbuf();
	string s = ss.str();
	ifs.close();
	char str[s.size()+1];
	strcpy(str, s.c_str());
	char * pch;
	pch = strtok(str, "~*~");
	while(pch != nullptr){
		vec.push_back(pch);
		pch = strtok(nullptr, "~*~");
	}
	*/
}

void displayTweets(const string& username, int connfd){
	//Goes through a vector and send each tweet to socket
	vector<string> vec;
	vector<string>::reverse_iterator it;
	parseMessage(username, vec);
	for(it = vec.rbegin(); it != vec.rend(); it++){
		cout << *it << endl;
		string line = *it + "~*~";
		send(connfd, line.c_str(), strlen(line.c_str()), 0);
	}
}

void tweet(const string& username, const string& date, const string& time1, const string& message, int connfd){
	//Stores tweet and also sends the tweets to the socket to be displayed
	ofstream ofs(username + "_messages.txt", ofstream::app);
	ofs << date + " " + time1 + " -- " + message + "\n";
	ofs.close();
	displayTweets(username, connfd);
}

string numFollowers(const string& username){
	//Returns the number of followers that the user has
	ifstream ifs(username + "_followers.txt");
	string line;
	int count = 0;
	while(getline(ifs,line))
		++count;
	stringstream ss;
	ifs.close();
	ss << count;
	ss >> line;
	return line;
}

void parseMessage2(const string& username, map<string, string>& myMap){
	//Parses messages and stores in map
	ifstream ifs(username + "_messages.txt");
	string line;
	while(getline(ifs,line)){
		myMap[line] = username;
	}
	ifs.close();
	/*
	ostringstream ss;
	ss << ifs.rdbuf();
	string s = ss.str();
	ifs.close();
	char str[s.size()+1];
	strcpy(str, s.c_str());
	char * pch;
	pch = strtok(str, "~*~");
	while(pch != nullptr){
		myMap[pch] = username;
		pch = strtok(nullptr, "~*~");
	}
	*/
}

void aggregateFeed(const string& username, int connfd){
	//stores all messages in a map and sends the most recent 15 messages
	map<string, string> myMap;
	map<string, string>::reverse_iterator it;
	ifstream ifs(username + "_followed.txt");
	string line;
	parseMessage2(username, myMap);
	while(getline(ifs,line)){
		parseMessage2(line, myMap);
	}
	ifs.close();
	//Sends the most recent 15 tweets to socket
	int count = 0;
	for(it = myMap.rbegin(); it != myMap.rend(); it++){
		if(count == 15) break;
		cout << it->second << " " << it->first << endl;
		string newMsg = it->second + " " + it->first + "~*~";
		count++;
		send(connfd, newMsg.c_str(), strlen(newMsg.c_str()), 0);
	}	
}

string requestHandler(const string& message, int connfd){
	//This handles all the use cases
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
		string user, date, time1, message, filler;
		iss >> user >> date >> time1 >> message;
		while(iss >> filler){
			message += ' ';
			message += filler;
		}
		tweet(user, date, time1, message, connfd);
		return("Tweeted");
	}
	else if( request == "num_followers" ){
		string user;
		iss >> user;
		return numFollowers(user);
	}
	else if( request == "aggregate_feed"){
		string user;
		iss >> user;
		aggregateFeed(user, connfd);
		return("Aggregate feed");
	}
}