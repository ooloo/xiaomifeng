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

def _add2xml(link, title, brand, price, category, img, bigimg, size):
  if brand == '':
    brand = 'other'

  dict = {}
  dict['item'] = {}
  dict['item']['link'] = link
  dict['item']['title'] = title
  dict['item']['brand'] = brand
  dict['item']['price'] = price.replace('￥','')
  dict['item']['category'] = category
  dict['item']['img'] = urlparse.urljoin(link, img).replace('/../', '/') 
  dict['item']['bigimg'] = urlparse.urljoin(link, bigimg).replace('/../', '/') 
  dict['item']['size'] = size
  dict['item']['store'] = sys.argv[2] 
  fo.write(dict2Xml(dict, "", 1))

#------------------ amazon ----------------
def _amazon(link, html):
  title=brand=price=img=size=category=''
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

  nodes=doc.xpath("//div[@class='ContentText']/a")
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    if(len(str) > 12):
      continue
    if(category == ''):
      category = str
    else:
      category = category + ',' + str
    print category

  y = unicode('容量：', 'utf8')
  nodes=doc.xpath("//div[@class='Left']//text()")
  try:
    x = nodes.index(y) + 1
    size = nodes[x].encode('utf8').strip() 
    print size
  except ValueError:
    pass

  bigimg = img.replace('AA200', 'AA500')
  _add2xml(link, title, brand, price, category, img, bigimg, size)
  #nodes=doc.xpath("//h2[@class='DetailTitle']")
  #for node in nodes:
    #tree=lxml.etree.ElementTree(node)
    #print tree.getpath(node)
    #print node.attrib
    #print node.text_content().encode('utf8').strip()

#------------------ 2688 ----------------
def _2688(link, html):
  title=brand=price=img=size=category=''
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

  nodes=doc.xpath("//div[@id='Product_Info_Top']/div[@class='Product_Type']/a")
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    if(len(str) > 12):
      continue
    if(category == ''):
      category = str
    else:
      category = category + ',' + str
    print category 

  nodes=doc.xpath("//div[@id='Product_MainPic']/img")
  try:
    img = nodes[0].attrib['src']
    print img
  except IndexError:
    pass

  bigimg = img.replace('_S.', '.')
  _add2xml(link, title, brand, price, category, img, bigimg, size)

#------------------ dangdang ----------------
def _dangdang(link, html):
  title=brand=price=img=size=category=''
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
      brand = node.text_content().encode('utf8').split('：')[-1].strip()
      print brand 
    elif node.text_content().find(y) >= 0:
      efficacy = node.text_content().encode('utf8').split('：')[-1].strip()
      print efficacy 

  nodes=doc.xpath("//div[@class='mall_your_position']/span/a")
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    if(len(str) > 12 or str.find('当当网') >= 0):
      continue
    if(category == ''):
      category = str
    else:
      category = category + ',' + str
    print category 

  nodes=doc.xpath("//img[@id='largePic']")
  try:
    img = nodes[0].attrib['src']
    print img
  except IndexError:
    pass

  bigimg = img.replace('_b.', '_o.')
  _add2xml(link, title, brand, price, category, img, bigimg, size)

#------------------ 360buy ----------------
def _360buy(link, html):
  title=brand=price=img=size=category=''
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

#------------------ no5 ----------------
def _no5(link, html):
  title=brand=price=img=size=category=''
  html = html.decode('gb18030')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//div[@id='history']/a")
  for node in nodes:
    brand = node.text_content().encode('utf8').strip()
    print brand 

  nodes=doc.xpath("//td[@class='no5_price']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//div[@id='history']/span[@class='redfont12']")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("//div[@id='top_pro']//tr/td")
  for node in nodes:
    #print node.attrib
    size = node.text_content().encode('utf8').strip()
    print size

  nodes=doc.xpath("//div[@id='top_pro']//td[@width='100%']/a")
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    if(len(str) > 12):
      continue
    if(category == ''):
      category = str
    else:
      category = category + ',' + str
    print category 

  nodes=doc.xpath("//div[@id='middle_pic_show']/a/img")
  try:
    img = nodes[0].attrib['src'].strip()
    print img
  except IndexError:
    pass

  _add2xml(link, title, brand, price, category, img, img, size)

#------------------ yoyo18 ----------------
def _yoyo18(link, html):
  title=brand=price=img=size=category=''
  html = html.decode('gb18030')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//div[@class='de_topPP']//font[@color='red']")
  for node in nodes:
    brand = node.text_content().encode('utf8').strip()
    print brand 

  nodes=doc.xpath("//div[@class='detrc_text']/font")
  for node in nodes:
    price = node.text_content().split()[0].encode('utf8').strip()
    print price

  nodes=doc.xpath("//div[@class='de_topT']/a/b")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("//div[@class='de_topT']/a")
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    if(len(str) > 12):
      continue
    if(category == ''):
      category = str
    else:
      category = category + ',' + str
    print category 

  nodes=doc.xpath("//input[@id='metricsSelect']/parent::*")
  for node in nodes:
    size = node.text_content().encode('utf8').strip()
    print size 

  nodes=doc.xpath("//div[@class='de_topImg']//a/img")
  try:
    img = nodes[0].attrib['src']
    print img
  except IndexError:
    pass

  nodes=doc.xpath("//div[@class='viewProductimg']/img")
  try:
    bigimg = nodes[0].attrib['src']
    print bigimg
  except IndexError:
    pass

  _add2xml(link, title, brand, price, category, img, bigimg, size)

#------------------ redbaby ----------------
def _redbaby(link, html):
  title=brand=price=img=size=category=''
  html = html.decode('gb18030')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//div[@id='ware_txt1']/h1")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("//div[@id='ware_mine']//span[@id='price']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//div[@id='ware_mine']/dl/dt[1]")
  for node in nodes:
    brand = node.text_content().encode('utf8').split('：')[-1].strip()
    print brand 

  nodes=doc.xpath("//div[@id='ware_mine']/dl/dt[2]")
  for node in nodes:
    size = node.text_content().encode('utf8').split('：')[-1].strip()
    print size 
  
  nodes=doc.xpath("//div[@id='location_info']/a")
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    if(len(str) > 12):
      continue
    if(category == ''):
      category = str
    else:
      category = category + ',' + str
    print category 

  nodes=doc.xpath("//div[@id='Product_BigImage']/a/img")
  try:
    img = nodes[0].attrib['src']
    print img
  except IndexError:
    pass

  bigimg = img.replace('Middle', 'Big')
  _add2xml(link, title, brand, price, category, img, bigimg, size)

#------------------ 7shop24 ----------------
def _7shop24(link, html):
  title=brand=price=img=size=category=''
  html = html.decode('gb18030')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//div[@class='contright']/h3")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("//div[@class='cproright']/p/span[@id='se']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//div[@class='cproduct']//a/img")
  try:
    img = nodes[0].attrib['src']
    print img
  except IndexError:
    pass
 
  nodes=doc.xpath("//div[@class='weizhi']/a")
  brand = nodes[-1].text_content().encode('utf8').strip()
  del nodes[-1]
  print brand 
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    if(len(str) > 12 or str.find('首页') >= 0):
      continue
    if(category == ''):
      category = str
    else:
      category = category + ',' + str
    print category 

  _add2xml(link, title, brand, price, category, img, img, size)

#------------------ lafaso ----------------
def _lafaso(link, html):
  title=brand=price=img=bigimg=size=category=''
  html = html.decode('utf8')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//div[@class='productdetail_row1right fl']/h2")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("//form[@id='productForm']//span[@class='red size14']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//div[@id='productdetail_row3_cpgg']//td/a")
  for node in nodes:
    brand = node.text_content().encode('utf8').strip()
    print brand 

  nodes=doc.xpath("//div[@class='productdetail_rowleft_bigpic']/img")
  try:
    img = nodes[0].attrib['src']
    print img
    bigimg = nodes[0].attrib['alt']
    print bigimg
  except IndexError:
    pass

  nodes=doc.xpath("//div[@class='location']/a")
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    if(len(str) > 12 or str.find('首页') >= 0):
      continue
    if(category == ''):
      category = str
    else:
      category = category + ',' + str
    print category 

  _add2xml(link, title, brand, price, category, img, bigimg, size)

#------------------ meixiu ----------------
def _meixiu(link, html):
  title=brand=price=img=size=category=''
  html = html.decode('gb18030')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//th")
  for node in nodes:
    #tree=lxml.etree.ElementTree(node)   
    #print tree.getpath(node)  
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("/html/body/table[2]/tr[1]/td[2]/table/tr[3]/td[2]/table/tr[4]//strong")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  #nodes=doc.xpath("/html/body/table[2]/tr[1]/td[2]/table/tr[3]/td[2]/table/tr[6]/td[2]")
  #for node in nodes:
    #brand = node.text_content().encode('utf8').strip()
    #print brand 

  nodes=doc.xpath("/html/body/table[2]/tr[1]/td[2]/table/tr[3]/td[1]/table/tr/td/img")
  try:
    img = nodes[0].attrib['src']
    print img
  except IndexError:
    pass

  nodes=doc.xpath("/html/body/table[2]/tr[1]/td[2]/table/tr[1]/td[1]/a")
  brand = nodes[-1].text_content().encode('utf8').strip()
  del nodes[-1]
  print brand 
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    if(len(str) > 12):
      continue
    if(category == ''):
      category = str
    else:
      category = category + ',' + str
    print category 

  _add2xml(link, title, brand, price, category, img, img, size)

#------------------ strawberrynet ----------------
def _strawberrynet(link, html):
  title=brand=price=img=size=category=''
  html = html.decode('gb18030').replace('&nbsp;', ' ')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//div[@class='div_content19 fontBold']")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("//div[@class='div_left div_content33 fontBold']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//div[@class='prodDetailPadding']/div/h3")
  for node in nodes:
    brand = node.text_content().encode('utf8').strip()
    print brand 

  nodes=doc.xpath("//div[@class='div_width395 div_centerAlign']/img")
  try:
    img = nodes[0].attrib['src']
    print img
  except IndexError:
    pass

  nodes=doc.xpath("//div[@class='mainPadding fontBold']/h2")
  for node in nodes:
    category = node.text_content().replace('-',',').replace(' ', '').encode('utf8').strip()
    print category 

  _add2xml(link, title, brand, price, category, img, img, size)

#------------------ m18 ----------------
def _m18(link, html):
  title=brand=price=img=bigimg=size=category=''
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//h1[@id='styleName']")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("//span[@id='stylePrice']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//div[@id='styleBrand']")
  for node in nodes:
    brand = node.text_content().encode('utf8').strip()
    print brand 

  nodes=doc.xpath("//div[@class='bigpic fl']/img")
  try:
    img = nodes[0].attrib['src']
    print img
  except IndexError:
    pass

  nodes=doc.xpath("//div[@class='goods-pic']/img")
  try:
    bigimg = nodes[0].attrib['src']
    print bigimg
  except IndexError:
    pass

  nodes=doc.xpath("//div[@class='crumb']/a")
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    if(len(str) > 12 or str.find('首页') >= 0):
      continue
    if(category == ''):
      category = str
    else:
      category = category + ',' + str
    print category 

  _add2xml(link, title, brand, price, category, img, bigimg, size)

#------------------ sasa ----------------
def _sasa(link, html):
  title=brand=price=img=bigimg=size=category=''
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//a[@class='txt_12px_n_666666']")
  title = nodes[0].text_content().encode('utf8').strip()
  print title 

  nodes=doc.xpath("//td[@class='txt_11px_b_EB6495']")
  price = nodes[1].text_content().encode('utf8').split()[-1]
  price = str(float(price) * 6.8)
  print price 

  nodes=doc.xpath("//a[@class='txt_13px_b_666666']")
  for node in nodes:
    brand = node.text_content().encode('utf8').strip()
    print brand 

  nodes=doc.xpath("//td[@width='50' and @class='txt_12px_b_666666']/parent::*")
  size = nodes[0].text_content().split()[1] + nodes[0].text_content().split()[2]
  size = size.encode('utf8').strip()
  print size 

  nodes=doc.xpath("//td[@width='350' and @align='center']/img")
  try:
    img = nodes[0].attrib['src']
    print img
  except IndexError:
    pass

  bigimg = img.replace('n.jpg', 'l.jpg')
  _add2xml(link, title, brand, price, category, img, bigimg, size)

#------------------ guopi ----------------
def _guopi(link, html):
  title=brand=price=img=bigimg=size=category=''
  #html = html.decode('gb18030')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//table[@class='fw_layout pad1']/tr/td")
  for i in range(len(nodes)):
    if nodes[i].text_content().encode('utf8').find('货品名') >= 0:
      title = nodes[i+1].text_content().encode('utf8').strip()
      print title 
    if nodes[i].text_content().encode('utf8').find('规格') >= 0:
      size = nodes[i+1].text_content().encode('utf8').strip()
      print size 
    if nodes[i].text_content().encode('utf8').find('产品系列') >= 0:
      category = nodes[i+1].text_content().encode('utf8').split()[0].replace('/',',').strip()
      print category 
    if nodes[i].text_content().encode('utf8').find('果皮价') >= 0:
      price = nodes[i+1].text_content().encode('utf8').strip()
      print price

  nodes=doc.xpath("//a[@class='blueline']")
  for node in nodes:
    brand = node.text_content().split()[0].encode('utf8').strip()
    print brand 

  nodes=doc.xpath("//a[@title]/img")
  try:
    img = nodes[0].attrib['src']
    print img
  except IndexError:
    pass

  nodes=doc.xpath("//a[@title]")
  try:
    bigimg = nodes[0].attrib['href']
    print bigimg
  except IndexError:
    pass

  _add2xml(link, title, brand, price, category, img, bigimg, size)

#------------------ yihaodian ----------------
def _yihaodian(link, html):
  title=brand=price=img=bigimg=size=category=''
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//span[@id='product_name']")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("//span[@id='product_list_price']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//span[@id='product_format']")
  for node in nodes:
    size = node.text_content().encode('utf8').strip()
    print size 

  nodes=doc.xpath("//div[@class='list_s']/div/a")
  brand = nodes[1].text_content().encode('utf8').strip()
  if brand.find('1号店') >= 0:
    brand = ''
  print brand 

  nodes=doc.xpath("//span[@id='picDiv']//img")
  try:
    img = nodes[0].attrib['src']
    print img
    bigimg = nodes[0].attrib['alt']
    print bigimg
  except IndexError:
    pass

  nodes=doc.xpath("//div[@class='nav blueT']/a")
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    if(len(str) > 12 or str.find('一号店') >= 0):
      continue
    if(category == ''):
      category = str
    else:
      category = category + ',' + str
    print category

  _add2xml(link, title, brand, price, category, img, bigimg, size)

#------------------ meethall ----------------
def _meethall(link, html):
  title=brand=price=img=size=category=''
  html = html.decode('gb18030').replace('&nbsp;&nbsp;', ' ')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//div[@class='info']/ul/li[1]/strong")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("//div[@class='info']/ul/li[4]/a")
  for node in nodes:
    brand = node.text_content().encode('utf8').strip()
    print brand

  nodes=doc.xpath("//div[@class='meethallprice']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//font[@color='chocolate']")
  for node in nodes:
    category = node.text_content().encode('utf8').replace('/', ',').strip()
    print category
    break

  nodes=doc.xpath("//div[@class='info']/ul/li")
  for i in range(len(nodes)):
    if nodes[i].text_content().encode('utf8').find('规 格') >= 0:
      size = nodes[i].text_content().encode('utf8').split('：')[-1].strip()
      print size 
      break

  nodes=doc.xpath("//div[@class='datu']/img")
  try:
    img = nodes[0].attrib['src'].strip()
    print img
  except IndexError:
    pass

  _add2xml(link, title, brand, price, category, img, img, size)


#------------------ main ----------------
if __name__ == '__main__': 
  if(len(sys.argv) != 3):
    print 'Usage: ' + sys.argv[0] + ' <datapath> <site>' 
    exit(1)

  global fo
  fo = open('/baijia/' + sys.argv[2] + '.xml', 'w')
  fo.write('<?xml version="1.0" encoding="UTF-8"?>\n<items>\n')
  linkdb = bsddb.btopen(sys.argv[1] + '._link.bdb', 'r')

  for k, v in linkdb.iteritems():
    #print '>>', k, v
    print '>>', v
    try:
      f = open(sys.argv[1] + k + '.html', 'r')
      html = f.read()
      f.close()
    except IOError, e:
      print 'No such file or directory'

    t=eval("_" + sys.argv[2])
    try:
      t(v, html)
    except Exception, e:  
      pass
   # break

  linkdb.close()
  fo.write('</items>\n')
  fo.close()
