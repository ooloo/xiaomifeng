import pycurl,string
import hashlib,urlparse
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
  finally:
    filesize = ' [' + str(fp.tell()) + ']'
    fp.close()

  code = c.getinfo(pycurl.HTTP_CODE)
  print 'save to file: ' + filename + filesize

  if(code == 200):
    return True
  else:
    if os.path.isfile(filename):
      os.remove(filename)
    return False


if(len(sys.argv) != 3):
  print 'Usage: ' + sys.argv[0] + ' <linkfile> <savepath>'
  exit(1)

file=open(sys.argv[1], 'r')
linkList = file.readlines()
file.close()

savePath=sys.argv[2] + '/'

c = pycurl.Curl()
c.setopt(pycurl.FOLLOWLOCATION, 1)
c.setopt(pycurl.CONNECTTIMEOUT, 30)
c.setopt(pycurl.TIMEOUT, 15)
c.setopt(pycurl.MAXREDIRS, 3)
c.setopt(pycurl.NOSIGNAL, 1)
c.setopt(pycurl.HTTPHEADER, ['Connection: keep-alive','Accept: */*',
  'User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1)'])

linkdb = bsddb.btopen(savePath + '._link.bdb', 'c')
timedb = bsddb.btopen(savePath + '._time.bdb', 'c')

for line in linkList:
  tmp = line.split()
  link = tmp[0]
  print '>>', link 
  m1 = hashlib.md5()
  m1.update(link)
  key = m1.hexdigest()

  now = time.time()
  if timedb.has_key(key):
    update = now - string.atof(timedb[key])
  else:
    update = now

  if update < 86400.0 * 3:
    print 'this key no need update: ' + key
  else:
    if httpGet(link, key):
      linkdb[key] = link
      linkdb.sync()
    time.sleep(1)

  timedb[key] = str(now)
  timedb.sync()

linkdb.close()
timedb.close()

