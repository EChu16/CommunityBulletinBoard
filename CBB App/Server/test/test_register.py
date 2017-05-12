#!/usr/bin/python
from requests.packages.urllib3.exceptions import InsecureRequestWarning
import requests as r
import json
r.packages.urllib3.disable_warnings(InsecureRequestWarning)

username = "djn263"
port = '8081'
base_url = 'https://' + username + '.code.engineering.nyu.edu:' + port + '/'

register_payload = {"username" : "john", "password" : "1234"}

r1 = r.post(base_url + 'user/register', verify=False, json=register_payload)
print "Response from register: " + r1.content
if r1.status_code == 200:
	print "Session_id cookie: " + r1.cookies["session_id"]
	print r1.content + "\n"