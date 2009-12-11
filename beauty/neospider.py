import pycurl
import md5,urlparse
import time,StringIO
import sys, os, re 
import bsddb

def httpGet(url, key):
	host, path, params = urlparse.urlsplit(url)[1:4]
	if ':' in host:
		# port specified, try to use it
		host, port = host.split(':', 1)

	filename = savePath + key + '.html'
	fp = open(filename, 'w')

	c.setopt(pycurl.URL, url)
	c.setopt(pycurl.WRITEDATA, fp)
	try:
		c.perform()
	except Exception, e:
		print e.__class__, e, url
		return False
	code = c.getinfo(pycurl.HTTP_CODE)
	filesize = ' [' + str(fp.tell()) + ']'
	fp.close()
	print 'save to file: ' + filename + filesize

	return True 


if(len(sys.argv) != 3):
	print 'Usage: ' + sys.argv[0] + ' <linkfile> <savepath>'
	exit(1)

file=open(sys.argv[1], 'r')
linkList = file.readlines()
file.close() 

savePath=sys.argv[2]

c = pycurl.Curl()
c.setopt(pycurl.FOLLOWLOCATION, 1)
c.setopt(pycurl.CONNECTTIMEOUT, 30)
c.setopt(pycurl.MAXREDIRS, 3)
c.setopt(pycurl.HTTPHEADER, ['Connection: keep-alive',
								'User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1)',
								'Keep-Alive: 300'])

linkdb = bsddb.btopen(savePath + '/._link.bdb', 'c')

for line in linkList:
	tmp = line.split()
	link = tmp[0]
	print '>>', link 
	m1 = md5.new()
	m1.update(link)
	key = m1.hexdigest()

	if linkdb.has_key(key):
		print 'this key id exist: ' + key
	else:
		httpGet(link, key)
		linkdb[key] = link
		linkdb.sync()
		time.sleep(1)

linkdb.close()

