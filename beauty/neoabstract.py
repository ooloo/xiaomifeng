#!/usr/bin/python
# -*- coding: utf8 -*-

import urlparse
import time,StringIO
import sys, os, re 
import bsddb
import lxml.html
import lxml.etree
import lxml.html as H
from _dict2xml import dict2Xml

def _add2xml(link, title, brand, price, img, size):
  dict = {}
  dict['item'] = {}
  dict['item']['link'] = link
  dict['item']['title'] = title 
  dict['item']['brand'] = brand 
  dict['item']['price'] = price 
  dict['item']['img'] = img 
  dict['item']['size'] = size
  fo.write(dict2Xml(dict, "", 1))

#------------------ amazon ----------------
def _amazon(link, html):
  title=brand=price=img=size=''
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//span[@class='SalePrice']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price

  nodes=doc.xpath("//h1[@class='DetailTitle']")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title

  nodes=doc.xpath("//div[@class='product-author']/a")
  for node in nodes:
    brand = node.text_content().encode('utf8').strip()
    print brand

  nodes=doc.xpath("//div[@id='productshowmidpic']/a/img")
  try:
    img = nodes[0].attrib['src']
    print img
  except IndexError:
    pass

  y = unicode('容量：', 'utf8')
  nodes=doc.xpath("//div[@class='Left']//text()")
  try:
    x = nodes.index(y) + 1
    size = nodes[x].encode('utf8').strip() 
    print size
  except ValueError:
    pass

  _add2xml(link, title, brand, price, img, size)
  #nodes=doc.xpath("//h2[@class='DetailTitle']")
  #for node in nodes:
    #tree=lxml.etree.ElementTree(node)
    #print tree.getpath(node)
    #print node.attrib
    #print node.text_content().encode('utf8').strip()

#------------------ 2688 ----------------
def _2688(link, html):
  title=brand=price=img=size=''
  html = html.decode('gb18030')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//li/em")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//div[@class='Product_Name']")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title

  nodes=doc.xpath("//div[@class='Product_Brand']/a")
  for node in nodes:
    brand = node.text_content().encode('utf8').strip()
    print brand

  nodes=doc.xpath("//div[@id='Product_MainPic']/img")
  try:
    img = nodes[0].attrib['src']
    print img
  except IndexError:
    pass

  _add2xml(link, title, brand, price, img, size)

#------------------ dangdang ----------------
def _dangdang(link, html):
  title=brand=price=img=size=''
  html = html.decode('gb18030')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//b[@id='salePriceTag']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//div[@class='mall_goods_title_text']")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title

  x = unicode('品牌：', 'utf8')
  y = unicode('功效：', 'utf8')
  nodes=doc.xpath("//div[@class='mall_goods_foursort_style_frame']")
  for node in nodes:
    if node.text_content().find(x) >= 0:
      brand = node.text_content().encode('utf8').strip()
      print brand 
    elif node.text_content().find(y) >= 0:
      efficacy = node.text_content().encode('utf8').strip()
      print efficacy 

  nodes=doc.xpath("//img[@id='largePic']")
  try:
    img = nodes[0].attrib['src']
    print img
  except IndexError:
    pass

  _add2xml(link, title, brand, price, img, size)

#------------------ 360buy ----------------
def _360buy(link, html):
  title=brand=price=img=size=''
  html = html.decode('gb18030')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//div[@class='Product_Name']/h1")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title

  nodes=doc.xpath("//em[@class='m_line']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//div[@class='margin_b6' and @id='Position']/a")
  for node in nodes:
    brand = node.text_content().encode('utf8').strip()
    print brand 

  nodes=doc.xpath("//div[@id='EFF_PINFO_Con_1']")
  for node in nodes:
    size = node.text_content().encode('utf8').strip()
    print size 

  nodes=doc.xpath("//div[@id='Product_BigImage']/img")
  try:
    img = nodes[0].attrib['src']
    print img
  except IndexError:
    pass

  _add2xml(link, title, brand, price, img, size)

#------------------ xxx ----------------
def xxx(link, html):
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

  global fo
  fo = open('/tmp/' + sys.argv[2] + '.xml', 'w')
  fo.write('<?xml version="1.0" encoding="UTF-8"?>\n<items>\n')
  linkdb = bsddb.btopen(sys.argv[1] + '._link.bdb', 'r')

  for k, v in linkdb.iteritems():
    #print '>>', k, v
    print '>>', v
    f = open(sys.argv[1] + k + '.html', 'r')
    html = f.read()
    f.close()

    t=eval("_" + sys.argv[2])
    t(v, html)

#    break

  linkdb.close()
  fo.write('</items>\n')
  fo.close()
