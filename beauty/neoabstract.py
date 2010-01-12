import urlparse
import time,StringIO
import sys, os, re 
import bsddb
import lxml.html
import lxml.html as H

#------------------ amazon ----------------
def amazon(html):
  doc = H.document_fromstring(html)
  nodes=doc.xpath("//span[@class='SalePrice']")
  for node in nodes:
    print node.text_content()

  nodes=doc.xpath("//h1[@class='DetailTitle']")
  for node in nodes:
    print node.text_content().encode('gb18030') 

  nodes=doc.xpath("//h1[@div='product-author']")
  for node in nodes:
    print node.text_content().encode('gb18030') 


#------------------ amazon ----------------
def amazon(html):
  doc = H.document_fromstring(html)
  nodes=doc.xpath("//span[@class='SalePrice']")
  for node in nodes:
    print node.text_content()

  nodes=doc.xpath("//h1[@class='DetailTitle']")
  for node in nodes:
    print node.text_content().encode('gb18030') 

  nodes=doc.xpath("//h1[@div='product-author']")
  for node in nodes:
    print node.text_content().encode('gb18030') 


#------------------ amazon ----------------
def amazon(html):
  doc = H.document_fromstring(html)
  nodes=doc.xpath("//span[@class='SalePrice']")
  for node in nodes:
    print node.text_content()

  nodes=doc.xpath("//h1[@class='DetailTitle']")
  for node in nodes:
    print node.text_content().encode('gb18030') 

  nodes=doc.xpath("//h1[@div='product-author']")
  for node in nodes:
    print node.text_content().encode('gb18030') 


#------------------ amazon ----------------
def amazon(html):
  doc = H.document_fromstring(html)
  nodes=doc.xpath("//span[@class='SalePrice']")
  for node in nodes:
    print node.text_content()

  nodes=doc.xpath("//h1[@class='DetailTitle']")
  for node in nodes:
    print node.text_content().encode('gb18030') 

  nodes=doc.xpath("//h1[@div='product-author']")
  for node in nodes:
    print node.text_content().encode('gb18030') 


#------------------ amazon ----------------
def amazon(html):
  doc = H.document_fromstring(html)
  nodes=doc.xpath("//span[@class='SalePrice']")
  for node in nodes:
    print node.text_content()

  nodes=doc.xpath("//h1[@class='DetailTitle']")
  for node in nodes:
    print node.text_content().encode('gb18030') 

  nodes=doc.xpath("//h1[@div='product-author']")
  for node in nodes:
    print node.text_content().encode('gb18030') 

#------------------ main ----------------
if __name__ == '__main__': 
  if(len(sys.argv) != 3):
    print 'Usage: ' + sys.argv[0] + ' <datapath> <site>' 
    exit(1)

  linkdb = bsddb.btopen(sys.argv[1] + '._link.bdb', 'r')

  for k, v in linkdb.iteritems():
    print '<<', v
    f = open(sys.argv[1] + k + '.html', 'r')
    html = f.read()
    f.close()

    t=eval(sys.argv[2])
    t(html)

    break
  linkdb.close()

