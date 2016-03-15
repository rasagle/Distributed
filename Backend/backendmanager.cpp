#include "backendmanager.h"
using namespace std;

string registerUser(string& username, string& password){
	
	ifstream ifs( username + ".txt" );
	if(!ifs){
		ofstream ofs( username + ".txt" );
		ofs << username + " " + password;
		ofs.close();
		ofstream ofs1( "userbase.txt", ofstream::app );
		ofs1 << username + "\n";
		ofs1.close();
		return("Register successful");
	}
	else{
		return("Username already exists");
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
}