#!/usr/bin/python
# -*- coding: utf8 -*-
# coding=gb18030

import re,string
import urlparse
import time,StringIO
import sys, os, re 
import lxml.html
import lxml.html as H
from _dict2xml import dict2Xml

def _add2xml(time, title, t1, t2):
  dict = {}
  dict['match'] = {}
  dict['match']['time'] = time
  dict['match']['title'] = title
  dict['match']['t1'] = t1
  dict['match']['t2'] = t2
  fo.write(dict2Xml(dict, "", 1))

#------------------ pclady ----------------
def _duandi(html):
  time=title=t1=t2=''
  html = html.decode('gb18030')
  doc = H.document_fromstring(html)

  i=0
  #nodes=doc.xpath("//div[@class='resultTables']")
  nodes=doc.xpath("//div[@class='todayDetailsBoxing']//div[@class='resultTables']//tr/td")
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    if(str.find('-') >= 0 and str.find(':') >= 0):
      time = str
      i=1
    if(i==2):
      title = str
    if(i==3):
      t1 = str.split()[0]
      t2 = str.split()[2] 
      _add2xml(time, title, t1, t2)
    i=i+1
    print "------------------" 
    print time+title+t1+t2

#------------------ main ----------------
if __name__ == '__main__':
  global fo,fd,spiderTime;
  ISOTIMEFORMAT='%Y%m%d'
  timestr = time.strftime(ISOTIMEFORMAT, time.localtime())
  #fo = open('/home3/xml/' + sys.argv[2] + '_' + timestr + '.xml', 'w')
  fo = open('/tmp/match.xml', 'w')

  now = time.time()
  fo.write('<?xml version="1.0" encoding="UTF-8"?>\n<matchs>\n')

  filename = './match'
      
  if os.path.isfile(filename):
    f = open(filename, 'r')
    html = f.read()
    f.close()
  else:
    sys.exit(0); 

  t=eval("_" + sys.argv[1])
  t(html)

  fo.write('</matchs>\n')
  fo.close()
