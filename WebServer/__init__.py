from flask import Flask, render_template, redirect, session, url_for, request, escape, flash
import os.path
import datetime
import time

app = Flask(__name__)

@app.route('/')
def index():
	''' This is the homepage of the app'''
	username = None
	num_follower = 0
	if 'username' in session:
		username = session['username']
		#This is to display the number of followers they have
		f = open(username + "_followers.txt", 'r')
		for line in f:
			num_follower += 1
	return render_template("index.html", message = username, num = num_follower)


def joinPath(file_name):
	'''Returns the path of the file that is passed in'''
	return os.path.join(os.path.dirname(__file__), file_name)
	
def checkValidUser(file_name, username, password):
	'''Checks to see if username and password match'''
	file = open(joinPath(file_name), 'r')
	line = file.readline()
	info = line.split(" ")
	if info[0] == username and info[1] == password:
		return True
	return False
	
def addUserbase(username):
	'''User is added to global list of users'''
	file = open(joinPath('userbase.txt'), 'a')
	file.write(username + '\n')
	file.close()

def removeUserbase(username):
	'''Deletes user from global list of users'''
	f = open("userbase.txt","r+")
	lines = f.readlines()
	f.seek(0)
	for line in lines:
	#Write if username does not equal line in file
		if line != (username + '\n'):
			f.write(line)
	f.truncate()
	f.close()

@app.route('/login', methods = ['GET', 'POST'])
def login():
	'''Login'''
	redirect_code = "login"
	error = None
	if request.method == 'POST':
		attempt_user = request.form['username']
		attempt_pass = request.form['password']
		#Checks that the file exists and user is valid
		if os.path.exists(joinPath(attempt_user + '.txt')) and \
		checkValidUser(attempt_user + '.txt', attempt_user, attempt_pass):
			session['username'] = attempt_user
			return render_template('redirect.html', redirect_code = redirect_code)
		error = "Username/Password does not match"
	return render_template("login.html", error = error)
	
def addUserFiles(username):
	'''Creates 3 files that is associated with each user'''
	#Creates a file that stores all the messages that user has shared
	f = open(joinPath(username + "_messages.txt"), 'w')
	f.close()
	#Creates a file that stores all the people that the user has followed
	f = open(joinPath(username + "_followed.txt"), 'w')
	f.close()
	#Creates a file that stores all the people that has followed the user
	f = open(joinPath(username + "_followers.txt"), 'w')
	f.close()
	
@app.route('/register', methods = ['GET', 'POST'])
def register():
	'''Registers the user into the system'''
	redirect_code = "register"
	error = None
	if request.method == 'POST':
		reg_user = request.form['username']
		reg_pass = request.form['password']
		#Checks to see if user already exists
		if os.path.exists(joinPath(reg_user + '.txt')):
			error = "This username already exists"
			return render_template("register.html", error = error)
		file_name = reg_user + '.txt'
		#Files are being created
		with open(joinPath(file_name), 'w') as fo:
			fo.write(reg_user + ' ' + reg_pass)
		addUserbase(reg_user)
		addUserFiles(reg_user)
		return render_template('redirect.html', redirect_code = redirect_code)
	return render_template("register.html", error = error)

@app.route('/logout')
def logout():
	'''Logs out of the system'''
	redirect_code = "logout"
	session.pop('username', None)
	return render_template("redirect.html", redirect_code = redirect_code)

def	remove_followers_info(username):
	'''When a user deletes the account, this function is called to go into their follower's
	files and delete the user's name from the file'''
	f = open(username + "_followers.txt","r+")
	for line in f:
		user = line[:-1]
		#Goes into your followed file, finds all the names and delete current user's name 
		#from a new file that is associated with each name in the current filefile
		if os.path.exists(joinPath(user + '_followed.txt')):
			fo = open(user + '_followed.txt', 'r+')
			lines = fo.readlines()
			fo.seek(0)
			for line in lines:
				if line != (username + '\n'):
					fo.write(line)
			fo.truncate()
			fo.close()
	f.close()
	f = open(username + "_followed.txt","r+")
	for line in f:
		user = line[:-1]
		#Goes into your follower file, finds all the names and delete current user's name 
		#from a new file that is associated with each name in the current filefile
		if os.path.exists(joinPath(user + '_followers.txt')):
			fo = open(user + '_followers.txt', 'r+')
			lines = fo.readlines()
			fo.seek(0)
			for line in lines:
				if line != (username + '\n'):
					fo.write(line)
			fo.truncate()
			fo.close()
	f.close()
	
@app.route('/remove_account', methods = ['GET', 'POST'])
def remove_account():
	'''Deletes an account'''
	error = None
	redirect_code = "remove"
	if request.method == "POST":
		errorMsg = request.form['verify']
		username = session['username']
		if errorMsg == "I am sure":
			remove_followers_info(username)
			#Each file is being deleted
			os.unlink(joinPath(username + '.txt'))
			os.unlink(joinPath(username + "_messages.txt"))
			os.unlink(joinPath(username + "_followed.txt"))
			os.unlink(joinPath(username + "_followers.txt"))
			#User removed from userbase
			removeUserbase(username)
			session.pop('username', None)
			return render_template("redirect.html", redirect_code = redirect_code)
		else:
			error = "You did not type the correct input"
	return render_template("remove_account.html", message = error)
	
@app.route('/tweets', methods = ['GET', 'POST'])
@app.route('/tweets/<username>', methods = ['GET', 'POST'])
def tweets(username = None):
	'''Allows users to post tweets'''
	isUser = False
	redirect_code = "invalid_user"
	if username == None:
		isUser = True
		username = session['username']
	#Checks to see if user deleted account, if not, proceed
	if os.path.exists(joinPath(username + "_messages.txt")):
		f = open(username + "_messages.txt", 'r')
	else:
		return render_template("redirect.html", redirect_code = redirect_code)
	
	lines = f.read()
	available = None
	if lines != "":
		available = True
		#Each comment in the file is separated with ~*~
		tweets = lines.split('~*~\n')
	else:
		tweets = []
	f.close()
	
	if request.method == "POST":
		#generates a time stamp when each tweet is made
		ts = time.time()
		time_stamp = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
		f = open(username + "_messages.txt", 'a+')
		f.write(time_stamp + ' -- ' + request.form['tweet'] + '~*~\n')
		f.close()
		f = open(username + "_messages.txt", 'r')
		lines = f.read()
		#parses the tweets so the html and read it
		tweets = lines.split('~*~\n')
		f.close()
		available = True
		
	return render_template("tweets.html", username = username, available = available, tweets = tweets, isUser = isUser)
	
def find_everyone():
	'''Finds everyone that the user is not following'''
	username = session['username']
	f = open('userbase.txt', 'r')
	rLines = f.readlines()
	cf = open(username + "_followed.txt", 'r')
	#Loads the global list of users and deletes everyone you are following
	for line in cf:
		if line in rLines:
			rLines.remove(line)
	user = username + '\n'
	#deletes current user from list as well
	if user in rLines:
		rLines.remove(user)
	f.close()
	cf.close()
	return rLines
	
@app.route('/find_people', methods = ['GET', 'POST'])
def find_people():
	'''Utilizes find_everyone then allows user to follow the person'''
	redirect_code = "find_people"
	userbase = find_everyone()
	if len(userbase) == 0:
		redirect_code = "no more people"
		return render_template("redirect.html", redirect_code = redirect_code)
	current_user = session['username']
	if request.method == "POST":
		#Once post request is made, all the necessary files are written
		username = request.form['username']
		f = open(current_user + "_followed.txt", 'a')
		f.write(username + '\n')
		f.close()
		f = open(username + "_followers.txt", 'a')
		f.write(current_user + '\n')
		f.close()
		return render_template("redirect.html", redirect_code = redirect_code)
	
	
	return render_template("find_people.html", userbase = userbase)

@app.route('/unfollow', methods = ['GET', 'POST'])
def unfollow():
	'''Unfollows a specified person'''
	redirect_code = "unfollow"
	username = session['username']
	f = open(username + '_followed.txt', 'r')
	lines = f.readlines()
	if len(lines) == 0:
		redirect_code = "no_followed"
		return render_template("redirect.html", redirect_code = redirect_code)
	if request.method == "POST":
		name = request.form['username']
		f = open(username + "_followed.txt","r+")
		lines = f.readlines()
		f.seek(0)
		#Deletes the name of person u want unfollowed
		for line in lines:
			if line != (name + '\n'):
				f.write(line)
		f.truncate()
		f.close()
		#Goes into that person's follower's file and delete current user's name
		if os.path.exists(joinPath(name + '_followers.txt')):
			f = open(name + "_followers.txt","r+")
			lines = f.readlines()
			f.seek(0)
			for line in lines:
				if line != (username + '\n'):
					f.write(line)
			f.truncate()
			f.close()
		else:
			return render_template("redirect.html", redirect_code = "invalid_user")
		return render_template("redirect.html", redirect_code = redirect_code)
	return render_template("unfollow.html", userbase = lines)
	
	
@app.route('/view_followed', methods = ['GET', 'POST'])
def view_followed():
	'''Displays everyone that you have followed and you can choose to view their tweets'''
	username = session['username']
	f = open(username + "_followed.txt", 'r')
	lines = f.readlines()
	f.close()
	#Displays everyone in your followed file
	if request.method == "POST":
		user = request.form['username']
		return redirect(url_for('tweets', username = user))
		
	return render_template('view_followed.html', user_list = lines)

app.secret_key = 'A0Zr98j/3yX R~XHH!jmN]LWX/,?RT'
if __name__ == "__main__":
	app.run("192.81.212.251", 5000, debug = True)
