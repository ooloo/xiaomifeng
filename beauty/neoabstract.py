import urlparse
import time,StringIO
import sys, os, re 
import bsddb
import lxml.html
import lxml.html as H

if(len(sys.argv) != 2):
	print 'Usage: ' + sys.argv[0] + ' <datapath>'
	exit(1)

linkdb = bsddb.btopen(sys.argv[1] + '._link.bdb', 'r')

for k, v in linkdb.iteritems():
  print '<<', k, v
  f = open(sys.argv[1] + k + '.html', 'r')
  html = f.read()
  f.close()

  doc = H.document_fromstring(html)
  nodes=doc.xpath('//a')
  for node in nodes:
    print node.attrib

  break

linkdb.close()

