from flask import Flask, render_template, redirect, session, url_for, request, escape, flash
import os.path
import datetime
import time
import socket

app = Flask(__name__)

ports = [13002, 12002, 11002]
sequence = 0

def connectSocket(request):
        '''connects to the socket'''
        global sequence
        sequence += 1
        request = str(sequence) + " " + request
   	s = socket.socket()         	# Create a socket object
	host = "192.81.212.251" 	# Where do you want to connect
	#port = 13002                	# port to connect to
        for port in ports:
                try:
                        s.connect((host, port))
                        s.send(request.encode())
                        message = (s.recv(1024).decode("utf8"))
                        break
                except:
                        continue
	s.close()                     	# Close the socket when done
	return message	

@app.route('/')
def index():
        ''''This is the homepage of the app'''
        username = None
        num_follower = ''
        tweets = []
        
        if 'username' in session:
                username = session['username']
                num_follower = connectSocket("num_followers " + username)
                tweets = find_everyone("aggregate_feed " + username)
                if tweets == "": tweets = []

        return render_template("index.html", message = username, num = num_follower, tweets = tweets)

@app.route('/register', methods = ['GET', 'POST'])
def register():
        '''Registers the user into the system'''
        redirect_code = "register"
	error = None
        if 'username' in session:
                return redirect(url_for('index'))
        if request.method == 'POST':
		reg_user = request.form['username']
		reg_pass = request.form['password']
                message = connectSocket("register " + reg_user + '*' + reg_pass)
                if message == "Username already exists":
                        return render_template("register.html", error = message)
                return render_template('redirect.html', redirect_code = redirect_code)
        return render_template("register.html", error = error)

@app.route('/login', methods = ['GET', 'POST'])
def login():
	'''Login'''
	redirect_code = "login"
	error = None
        if 'username' in session:
                return redirect(url_for('index'))
	if request.method == 'POST':
                attempt_user = request.form['username']
		attempt_pass = request.form['password']
                message = connectSocket("login " + attempt_user + '*' + attempt_pass)
                if message == "The user is not registered":
                        error = message
                elif message == "Invalid credentials":
                        error = message
                else:
                        session['username'] = attempt_user
			return render_template('redirect.html', redirect_code = redirect_code)
        return render_template("login.html", error = error)

@app.route('/logout')
def logout():
	'''Logs out of the system'''
	redirect_code = "logout"
	try: username = session['username']
        except: return redirect(url_for('index'))
	session.pop('username', None)
	return render_template("redirect.html", redirect_code = redirect_code)

@app.route('/remove_account', methods = ['GET', 'POST'])
def remove_account():
	'''Deletes an account'''
	error = None
	redirect_code = "remove"
        try: username = session['username']
        except: return redirect(url_for('index'))
        if request.method == "POST":
		errorMsg = request.form['verify']
		if errorMsg == "I am sure":
                        connectSocket("remove_account " + username)
                        session.pop('username', None)
                        return render_template("redirect.html", redirect_code = redirect_code)
                else:
                        error = "You did not type the correct input"
        return render_template("remove_account.html", message = error)

def find_everyone(request):
        '''Listens to socket for multiple packets and parses accordingly'''
        global sequence
        sequence += 1
        request = str(sequence) + " " + request
        username = session['username']
        s = socket.socket()             
	host = "192.81.212.251"         
	#port = 13002 
        for port in ports:
                try:
		        s.connect((host, port))
		        s.send(request.encode())
                        s.settimeout(2)
		        names = ""
		        for i in range(10):
		                message = s.recv(1024).decode("utf8")
		                if message != "": names += message
		                #print message
		        everyone = names.split('~*~')
		        everyone.pop()
		        if not everyone: return ""
			break
		except:
                        continue
        s.close()
        return everyone

@app.route('/find_people', methods = ['GET', 'POST'])
def find_people():
        '''Utilizes find_everyone then allows user to follow the person'''
        redirect_code = "find_people"
        try: current_user = session['username']
        except: return redirect(url_for('index'))
	userbase = find_everyone("find_everyone " + current_user)
	if len(userbase) == 0:
		redirect_code = "no more people"
		return render_template("redirect.html", redirect_code = redirect_code)
        
        if request.method == "POST":
                username = request.form['username']
                connectSocket("find_people " + current_user + " " + username)
                return render_template("redirect.html", redirect_code = redirect_code)
        return render_template("find_people.html", userbase = userbase)

@app.route('/unfollow', methods = ['GET', 'POST'])
def unfollow():
	'''Unfollows a specified person'''
	redirect_code = "unfollow"
	try: username = session['username']
        except: return redirect(url_for('index'))
        everyone = find_everyone("get_unfollow_name " + username)
        if len(everyone) == 0:
		redirect_code = "no_followed"
		return render_template("redirect.html", redirect_code = redirect_code)
        if request.method == "POST":
                name = request.form['username']
                connectSocket("unfollow " + username + " " + name)
        	return render_template("redirect.html", redirect_code = redirect_code)
	return render_template("unfollow.html", userbase = everyone)

@app.route('/view_followed', methods = ['GET', 'POST'])
def view_followed():
	'''Displays everyone that you have followed and you can choose to view their tweets'''
	try: username = session['username']
        except: return redirect(url_for('index'))
        everyone = find_everyone("view_followed " + username)
        if len(everyone) == 0:
                redirect_code = "no_followed"
                return render_template("redirect.html", redirect_code = redirect_code)
        if request.method == "POST":
		user = request.form['username']
		return redirect(url_for('tweets', username = user))
		
	return render_template('view_followed.html', user_list = everyone)

@app.route('/tweets', methods = ['GET', 'POST'])
@app.route('/tweets/<username>', methods = ['GET', 'POST'])
def tweets(username = None):
	'''Allows users to post tweets'''
	isUser = False
	redirect_code = "invalid_user"
	if username == None:
		isUser = True
		try: username = session['username']
                except: return redirect(url_for('index'))
        
        message = connectSocket("check_user_exists " + username)
        if message == "Does not exist":
                return render_template("redirect.html", redirect_code = redirect_code)
        
        available = None
        tweets = find_everyone("display_tweets " + username)
        if tweets != "": available = True

        if request.method == "POST":
                ts = time.time()
		time_stamp = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
                user_tweet = request.form['tweet']
                connectSocket("tweet " + username + " " + time_stamp + " " + user_tweet)
                tweets = find_everyone("display_tweets " + username)
                available = True
        return render_template("tweets.html", username = username, available = available, tweets = tweets, isUser = isUser)

app.secret_key = 'A0Zr98j/3yX R~XHH!jmN]LWX/,?RT'
if __name__ == "__main__":
	app.run("127.0.0.1", 5000, debug = True)
