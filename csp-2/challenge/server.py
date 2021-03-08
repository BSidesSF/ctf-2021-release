from flask import Flask, render_template, request, redirect, send_from_directory
from flask_csp.csp import csp_header
from werkzeug.middleware import proxy_fix
import requests
import urllib

app = Flask(__name__)
app.wsgi_app = proxy_fix.ProxyFix(app.wsgi_app)

# csp two cdnjs.cloudflare.com
@app.route('/')
@app.route('/csp-two')
@csp_header({'connect-src':"*",'default-src':"'self' 'unsafe-inline'",'script-src':"'self' cdnjs.cloudflare.com 'unsafe-eval'"})
def cspTwo():
	return render_template('csp-two.html')

@app.route('/csp-two-result', methods = ['POST','GET'])
@csp_header({'connect-src':"*",'default-src':"'self' 'unsafe-inline'",'script-src':"'self' cdnjs.cloudflare.com 'unsafe-eval'"})
def cspTwoResult():
	payload = "None"
	if request.method == 'POST':
		payload = request.form['payload']
		# todo point to webbot
		# use payload to prevent encoding issue we ran into last time
		r = requests.post('http://127.0.0.1:3000/submit',
		        data={'url':request.base_url + "?payload=" + urllib.quote(payload)})
	if request.method == 'GET' and 'admin' in request.cookies and request.cookies.get("admin") == u"54E0DA086734D3985318F11970AE03BD":
		payload = request.args.get('payload')
	return render_template('csp-two-result.html', payload = payload)

@app.route('/csp-two-flag', methods = ['GET'])
@csp_header({'connect-src':"*",'default-src':"'self'",'script-src':"'self' cdnjs.cloudflare.com"})
def cspTwoFlag():
	if 'admin' in request.cookies and request.cookies.get("admin") == u"54E0DA086734D3985318F11970AE03BD":
		return "CTF{Can_Still_Pwn}"
	else:
		return "Ah ah ah, you didn't say the magic word"

app.debug = True
app.run(host="0.0.0.0", port=8000)
