#coding=utf-8;
import sys, os, re
import urllib
file=open(sys.argv[1], 'r')
linkList = file.readlines()
file.close() 

file=open(sys.argv[1], 'w')
for line in linkList:
  url = urllib.quote(line.strip(), ':?=/') + '\n';
  file.write(url)
file.close() 

