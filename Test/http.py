import httplib
import sys

conn = httplib.HTTPConnection('localhost', 8080)
conn.request("GET", "/test.html")
resp = conn.getresponse()

if (resp.status != 200):
    print "HTTP Test Failed: Invalid status code. Expected 200, got ", resp.status
    sys.exit(1)

testFile = open('../HTTP/html/test.html', 'r')

if (resp.read() != testFile.read()):
    print "HTTP Test Failed: Invalid response data."
    sys.exit(1)
