#!/usr/bin/python
from requests.packages.urllib3.exceptions import InsecureRequestWarning
import requests as r
import json
r.packages.urllib3.disable_warnings(InsecureRequestWarning)

username = "djn263"
port = '8081'
base_url = 'https://' + username + '.code.engineering.nyu.edu:' + port + '/'

login_payload = {"username" : "john", "password" : "1234"}
userlist_payload = {"community_name" : "updog"}

r1 = r.post(base_url + 'user/login', verify=False, json=login_payload)
print "Response from login: " + r1.content
if r1.status_code == 200:
	print "Session_id cookie: " + r1.cookies["session_id"] + "\n"
	r2 = r.post(base_url + 'cbb/userlist', verify=False, cookies=r1.cookies, json=userlist_payload)
	# print r2.content
	print "Userlist query: " + str(userlist_payload) + '\n'
	string = r2.content
	string = string.lstrip('[')
	string = string.rstrip(']')
	string = string.split(',')
	for item in string:
		print item