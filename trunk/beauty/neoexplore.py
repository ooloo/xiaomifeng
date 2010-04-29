import hashlib,urlparse
import time,StringIO
import sys, os, re 
import bsddb

if(len(sys.argv) != 3):
  #print 'Usage: ' + sys.argv[0] + ' <linkfile> <explore.bdb>'
  exit(1)

file=open(sys.argv[1], 'r')
linkList = file.readlines()
file.close() 

exdb = bsddb.btopen(sys.argv[2], 'c')
oldNum = len(exdb)

failcount = 0

if len(linkList) == 0:
  if exdb.has_key('failcount'):
    failcount = int(exdb['failcount'])
  failcount += 1
  exdb['failcount'] = str(failcount) 
  exdb.close()
  if failcount <= 3:
    exit(0)
  else:
    exit(1)

exdb['failcount'] = str(failcount) 

for line in linkList:
  tmp = line.split()
  link = tmp[0]
  m1 = hashlib.md5()
  m1.update(link)
  key = m1.hexdigest()
  exdb[key] = link

newNum = len(exdb)
exdb.close()

if oldNum < newNum:
  exit(0)
else:
  exit(1)

