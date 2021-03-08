from flask import Flask, render_template, request, redirect, send_from_directory
from flask_csp.csp import csp_header
from werkzeug.middleware import proxy_fix
import requests
import urllib

app = Flask(__name__)
app.wsgi_app = proxy_fix.ProxyFix(app.wsgi_app)

# csp one (data uri) use cookie 54E0DA086734D3985318F11970AE03BD

@app.after_request
def apply_csp(response):
    response.headers["Content-Security-Policy"] = "default-src 'self' 'unsafe-inline' 'unsafe-eval'; script-src-elem 'self'; connect-src *"
    return response

@app.route('/')
@app.route('/csp-one')
def cspOne():
	return render_template('csp-one.html')

@app.route('/csp-one-result', methods = ['POST','GET'])
def cspOneResult():
	payload = "None"
	if request.method == 'POST':
		payload = request.form['payload']
		requests.post('http://127.0.0.1:3000/submit', data={'url': request.base_url + "?payload=" + urllib.quote(payload)})
	if request.method == 'GET' and 'admin' in request.cookies and request.cookies.get("admin") == u"54E0DA086734D3985318F11970AE03BD":
		payload = request.args.get('payload')
	elif request.method == 'GET':
	    app.logger.warning('GET request without valid admin cookie.')
	return render_template('csp-one-result.html', payload = payload)

@app.route('/csp-one-flag', methods = ['GET'])
def cspOneFlag():
	if 'admin' in request.cookies and request.cookies.get("admin") == u"54E0DA086734D3985318F11970AE03BD":
		return "CTF{Can_Send_Payloads}"
	else:
		return "Ah ah ah, you didn't say the magic word"

app.run(host='0.0.0.0', port=8000)
