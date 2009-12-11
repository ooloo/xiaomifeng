import httplib
import md5
import urlparse
import time 

HEADERS = {
			'User-Agent': 'Mozilla/4.0 (compatible; MSIE 6.0;Windows NT 5.0)',
			'Accept': '*/*',
			'Connection': 'Keep-Alive'
			}

dict = {}
httplib.HTTPConnection.debuglevel = 1
savePath='./'

def httpExists(url, key):
	host, path, params = urlparse.urlsplit(url)[1:4]
	print host, path, params 
	if ':' in host:
		# port specified, try to use it
		host, port = host.split(':', 1)
		try:
			port = int(port)
		except ValueError:
			print 'invalid port number %r' % (port,)
			return False
	else:
		# no port specified, use default port
		port = None
	try:
		if host in dict:
			connection = dict[host]
			print "aaaaaaaaaaaaaaa"
		else:
			connection = httplib.HTTPConnection(host, port=port)
			dict[host] = connection
			print "bbbbbbbbbbbbbb"

		if connection.request("GET", path +"?" +params, "", HEADERS):
			connection = httplib.HTTPConnection(host, port=port)
			dict[host] = connection
			print "cccccccccccccc"
		
		resp = connection.getresponse()
		data = resp.read()
		if resp.status == 200:
			# normal 'found' status
			found = True
			pic = open(savePath + key + '.jpg', 'w+b')
			pic.write(data)
			pic.close()
		elif resp.status == 302:
			# recurse on temporary redirect
			link = urlparse.urljoin(url,resp.getheader('location', ''))
			print "------------------", link
			found = httpExists(link, key)
		else:
			# everything else -> not found
			print "Status %d %s : %s" % (resp.status, resp.reason, url)
			found = False
	except Exception, e:
		print e.__class__, e, url
		found = False
	return found


file=open('test', 'r')
linkList = file.readlines()
file.close() 

for line in linkList:   
	tmp = line.split()
	link = tmp[0]
	key = tmp[1]
	print '>>', link 
	print '>>', key 
	m1 = md5.new()
	m1.update(link)
	print '>>', m1.hexdigest()

	httpExists(link, key)
	
	time.sleep(1)

