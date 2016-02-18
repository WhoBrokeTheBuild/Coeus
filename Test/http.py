import httplib
import sys

print "-- HTTP Tests --"
print "[1/2] Request test.html"

conn = httplib.HTTPConnection('localhost', 8080)
conn.request("GET", "/test.html")
resp = conn.getresponse()

if (resp.status != 200):
    print "Failed: Invalid status code. Expected 200, got ", resp.status
    sys.exit(1)

testFile = open('../HTTP/html/test.html', 'r')

if (resp.read() != testFile.read()):
    print "Failed: Invalid response data."
    sys.exit(1)

print "Succeeded"


print "[2/2] Invalid request"

conn = httplib.HTTPConnection('localhost', 8080)
conn.request("GET", "/thisshouldgivemea404")
resp = conn.getresponse()

if (resp.status != 404):
    print "Failed: Invalid status code. Expected 404, got ", resp.status
    sys.exit(1)

print "Succeeded"
