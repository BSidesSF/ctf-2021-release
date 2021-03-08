from flask import Flask, render_template, request, redirect, send_from_directory, make_response
from werkzeug.middleware import proxy_fix
import requests
import hashlib

app = Flask(__name__)
app.wsgi_app = proxy_fix.ProxyFix(app.wsgi_app)

@app.route('/')
def home():
	resp = make_response(render_template('home.html', admin=isAdmin(request), flag=getFlag(request)))
	if not haveCookies(request):
		resp.set_cookie('tm_admin',"0")
		result = hashlib.md5("anonymous".encode())
		hashStr = result.hexdigest()
		resp.set_cookie('tm_user',hashStr)
	return resp

def haveCookies(request):
	if 'tm_admin' in request.cookies and 'tm_user' in request.cookies:
		return True
	else:
		return False

def isAdmin(request):
	if 'tm_admin' in request.cookies and 'tm_user' in request.cookies:
		result = hashlib.md5("admin".encode())
		hashStr = result.hexdigest()
		if request.cookies.get('tm_admin') == "1" and request.cookies.get('tm_user') == hashStr:
			return True
		else: 
			return False
	else:
		return False

def getFlag(request):
	if isAdmin(request):
		return "CTF{iAmAdm1n}"
	else:
		return ""


app.run(host='0.0.0.0', port=8000)
