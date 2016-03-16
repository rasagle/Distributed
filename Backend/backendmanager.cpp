#include "backendmanager.h"
using namespace std;

void addUserFiles(string& username){
	ofstream ofs( username + "_messages.txt" );
	ofstream ofs1( username + "_followers.txt" );
	ofstream ofs2( username + "_followed.txt" );
	ofs.close();
	ofs1.close();
	ofs2.close();
}

string registerUser(string& username, string& password){
	
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

string loginUser(string& username, string& password){
	
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

string requestHandler(string& message){
	
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
}