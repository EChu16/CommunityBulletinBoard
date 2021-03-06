#!/usr/bin/python
from requests.packages.urllib3.exceptions import InsecureRequestWarning
import requests as r
import json
r.packages.urllib3.disable_warnings(InsecureRequestWarning)

username = "djn263"
port = '8081'
base_url = 'https://' + username + '.code.engineering.nyu.edu:' + port + '/'

login_payload = {"username" : "june", "password" : "ecb53f367b3541da3a14236d9a091f9bcc7a9903953f36577c2664fdd2a68a55"}
# login_payload = {"username" : "john", "password" : "1234"}
delete_post_payload = {"post_id" : "58fd0eddc1336770a901b748"}

r1 = r.post(base_url + 'user/login', verify=False, json=login_payload)
print "Response from login: " + r1.content
if r1.status_code == 200:
	print "Session_id cookie: " + r1.cookies["session_id"] + "\n"
	r2 = r.post(base_url + 'posts/delete', verify=False, cookies=r1.cookies, json=delete_post_payload)
	print r2.content
