#!/usr/bin/python
import requests as r
import logging

try:
    import http.client as http_client
except ImportError:
    # Python 2
    import httplib as http_client
http_client.HTTPConnection.debuglevel = 1

logging.basicConfig()
logging.getLogger().setLevel(logging.DEBUG)
requests_log = logging.getLogger("requests.packages.urllib3")
requests_log.setLevel(logging.DEBUG)
requests_log.propagate = True

from requests.packages.urllib3.exceptions import InsecureRequestWarning
# import requests as r
import json
r.packages.urllib3.disable_warnings(InsecureRequestWarning)

base_url = 'https://djn263.code.engineering.nyu.edu:8080/'

login_payload = {"username" : "john", "password" : "1234"}

r1 = r.post(base_url + 'user/login', verify=False, json=login_payload)
print "Response from login: " + r1.content
if r1.status_code == 200:
	print "Session_id cookie: " + r1.cookies["session_id"] + "\n"
	r2 = r.post(base_url + 'cbb/test', verify=False, cookies=r1.cookies)
	print r2.content
