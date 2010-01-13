#!/usr/bin/python
# -*- coding: utf8 -*-

import urlparse
import time,StringIO
import sys, os, re 
import bsddb
import lxml.html
import lxml.etree
import lxml.html as H

#------------------ amazon ----------------
def _amazon(html):
  doc = H.document_fromstring(html)
  nodes=doc.xpath("//span[@class='SalePrice']")
  for node in nodes:
    print node.text_content()

  nodes=doc.xpath("//h1[@class='DetailTitle']")
  for node in nodes:
    print node.text_content().encode('utf8').strip()

  nodes=doc.xpath("//div[@class='product-author']/a")
  for node in nodes:
    print node.text_content().encode('utf8').strip() 

  nodes=doc.xpath("//div[@id='productshowmidpic']/a/img")
  print nodes[0].attrib['src']

  y = unicode('容量：', 'utf8')
  nodes=doc.xpath("//div[@class='Left']//text()")
  try:
    x = nodes.index(y) + 1
    print nodes[x].encode('utf8').strip()
  except ValueError:
    pass

  #nodes=doc.xpath("//h2[@class='DetailTitle']")
  #for node in nodes:
    #tree=lxml.etree.ElementTree(node)
    #print tree.getpath(node)
    #print node.attrib
    #print node.text_content().encode('utf8').strip()

#------------------ 2688 ----------------
def _2688(html):
  html = html.decode('gb18030')
  doc = H.document_fromstring(html)
  nodes=doc.xpath("//li/em")
  for node in nodes:
    print node.text_content()

  nodes=doc.xpath("//div[@class='Product_Name']")
  for node in nodes:
    print node.text_content().encode('utf8') 

  nodes=doc.xpath("//div[@class='Product_Brand']/a")
  for node in nodes:
    print node.text_content().encode('utf8') 

  nodes=doc.xpath("//div[@id='Product_MainPic']/img")
  print nodes[0].attrib['src']

#------------------ dangdang ----------------
def _dangdang(html):
  doc = H.document_fromstring(html)
  nodes=doc.xpath("//b[@id='salePriceTag']")
  for node in nodes:
    print node.text_content()

  nodes=doc.xpath("//div[@class='mall_goods_title_text']")
  for node in nodes:
    print node.text_content().encode('utf8') 

  nodes=doc.xpath("//div[@class='mall_goods_foursort_style_frame']/a")
  for node in nodes:
    print node.text_content().encode('utf8') 

  nodes=doc.xpath("//img[@id='largePic']")
  if len(nodes) > 0:
    print nodes[0].attrib['src']


#------------------ xxx ----------------
def xxx(html):
  doc = H.document_fromstring(html)
  nodes=doc.xpath("//span[@class='SalePrice']")
  for node in nodes:
    print node.text_content()

  nodes=doc.xpath("//h1[@class='DetailTitle']")
  for node in nodes:
    print node.text_content().encode('utf8') 

  nodes=doc.xpath("//h1[@div='product-author']")
  for node in nodes:
    print node.text_content().encode('utf8') 


#------------------ xxx ----------------
def xxx(html):
  doc = H.document_fromstring(html)
  nodes=doc.xpath("//span[@class='SalePrice']")
  for node in nodes:
    print node.text_content()

  nodes=doc.xpath("//h1[@class='DetailTitle']")
  for node in nodes:
    print node.text_content().encode('utf8') 

  nodes=doc.xpath("//h1[@div='product-author']")
  for node in nodes:
    print node.attrib
    print node.text_content().encode('utf8') 

#------------------ main ----------------
if __name__ == '__main__': 
  if(len(sys.argv) != 3):
    print 'Usage: ' + sys.argv[0] + ' <datapath> <site>' 
    exit(1)

  linkdb = bsddb.btopen(sys.argv[1] + '._link.bdb', 'r')

  for k, v in linkdb.iteritems():
    #print '>>', k, v
    print '>>', v
    f = open(sys.argv[1] + k + '.html', 'r')
    html = f.read()
    f.close()

    t=eval("_" + sys.argv[2])
    t(html)

    #break
  linkdb.close()

