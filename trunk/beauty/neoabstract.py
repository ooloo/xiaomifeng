#!/usr/bin/python
# -*- coding: utf8 -*-

import re
import urlparse
import time,StringIO
import sys, os, re 
import bsddb
import lxml.html
import lxml.etree
import lxml.html as H
from _dict2xml import dict2Xml

def _add2xml(link, title, brand, price, category, img, bigimg, size, keywords, desc):
  if brand == '':
    return
  elif bigimg.find('.') < 0:
    return
  elif title == '':
    return

  price = price.replace(',','')

  dict = {}
  dict['item'] = {}
  dict['item']['link'] = link
  dict['item']['title'] = title
  dict['item']['brand'] = brand
  dict['item']['price'] = re.search('[0-9.]+', price).group() 
  dict['item']['category'] = category
  dict['item']['img'] = urlparse.urljoin(link, bigimg).replace('/../', '/') 
  #dict['item']['bigimg'] = urlparse.urljoin(link, bigimg).replace('/../', '/') 
  dict['item']['size'] = size
  dict['item']['store'] = sys.argv[2]
  dict['item']['keywords'] = keywords
  dict['item']['desc'] = desc
  dict['item']['avlid'] = 'true' 
  fo.write(dict2Xml(dict, "", 1))

#------------------ amazon ----------------
def _amazon(link, html):
  title=brand=price=img=size=category=keywords=desc=''
  html = html.decode('utf8')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//b[@class='priceLarge']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price

  nodes=doc.xpath("//span[@id='btAsinTitle']")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title

  nodes=doc.xpath("//h1[@class='parseasinTitle']/following::*[1]")
  for node in nodes:
    brand = node.text_content().encode('utf8').strip()
    print brand

  nodes=doc.xpath("//meta[@name='description']")
  for node in nodes:
    desc = node.attrib['content'].encode('utf8').strip()
    print desc

  nodes=doc.xpath("//meta[@name='keywords']")
  for node in nodes:
    keywords = node.attrib['content'].encode('utf8').strip()
    print keywords 

  nodes=doc.xpath("//td[@id='prodImageCell']/a/img")
  try:
    img = nodes[0].attrib['src']
    print img
  except IndexError:
    pass

  nodes=doc.xpath("//div[@class='content']//a")
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    if(len(str) > 12):
      continue
    if(len(str) < 1):
      category = ''
    if(category == ''):
      category = str
    else:
      category = category + ',' + str
    print category

  #y = unicode('容量：', 'utf8')
  #nodes=doc.xpath("//div[@class='Left']//text()")
  #try:
  #  x = nodes.index(y) + 1
  #  size = nodes[x].encode('utf8').strip() 
  #  print size
  #except ValueError:
  #  pass

  bigimg = img.replace('_SL500_AA280_', '_AA500_')
  _add2xml(link, title, brand, price, category, img, bigimg, size, keywords, desc)
  #nodes=doc.xpath("//h2[@class='DetailTitle']")
  #for node in nodes:
    #tree=lxml.etree.ElementTree(node)
    #print tree.getpath(node)
    #print node.attrib
    #print node.text_content().encode('utf8').strip()

#------------------ 2688 ----------------
def _2688(link, html):
  title=brand=price=img=size=category=keywords=desc=''
  html = html.decode('gb18030')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//span[@class='price2']")
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
  _add2xml(link, title, brand, price, category, img, bigimg, size, keywords, desc)

#------------------ dangdang ----------------
def _dangdang(link, html):
  title=brand=price=img=size=category=keywords=desc=''
  html = html.decode('gb18030')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//span[@id='salePriceTag']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//div[@class='h1_title']/h1")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title

  nodes=doc.xpath("//meta[@name='description']")
  for node in nodes:
    desc = node.attrib['content'].encode('utf8').strip()
    print desc

  nodes=doc.xpath("//meta[@name='keywords']")
  for node in nodes:
    keywords = node.attrib['content'].encode('utf8').strip()
    print keywords

  brand = keywords.split(',')[-1].encode('utf8')
  if brand == "":
    vs = keywords.split(',')
    for i in range(0, len(vs) - 1):
      str = vs[i].upper().encode('utf8')
      n = len(str)
      if n > 10:
        continue
      if cmp(str[0:n], title.upper()[0:n]) == 0:
        brand = vs[i]
        break
  print brand

  nodes=doc.xpath("//div[@class='dp_break']/a")
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
  _add2xml(link, title, brand, price, category, img, bigimg, size, keywords, desc)

#------------------ 360buy ----------------
def _360buy(link, html):
  title=brand=price=img=size=category=keywords=desc=''
  html = html.decode('gb18030')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//del")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price

  nodes=doc.xpath("//div[@class='crumb']/a")
  title = nodes[-1].text_content().encode('utf8').strip()
  del nodes[-1]
  print title
  brand = nodes[-1].text_content().encode('utf8').strip()
  del nodes[-1]
  print brand
  del nodes[-1]
  for i in range(2, len(nodes)):
    str = nodes[i].text_content().encode('utf8').strip()
    if(category == ''):
      category = str
    else:
      category += ',' + str

  nodes=doc.xpath("//meta[@name='description']")
  for node in nodes:
    desc = node.attrib['content'].encode('utf8').strip()
    #print desc

  nodes=doc.xpath("//div[@class='jqzoom']/img")
  try:
    img = nodes[0].attrib['src']
    print img
  except IndexError:
    pass

  _add2xml(link, title, brand, price, category, img, img, size, keywords, desc)

#------------------ no5 ----------------
def _no5(link, html):
  title=brand=price=img=size=category=keywords=desc=''
  html = html.decode('gb18030')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//div[@class='pro_text magenta']/span")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price

  nodes=doc.xpath("//div[@class='sp_p_texth']/p")
  for node in nodes:
    if(node.text_content() != ''):
      desc = node.text_content().encode('utf8').strip()
      break;

  nodes=doc.xpath("//div[@id='pro_text_pname']")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("//div[@class='subpage_title']/span/a")
  brand = nodes[-1].text_content().encode('utf8').strip()
  del nodes[-1]
  print brand
  del nodes[0]
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    if(category == ''):
      category = str
    else:
      category = category + ',' + str
    print category 

  nodes=doc.xpath("//div[@class='pro_pic_big']/a/img")
  try:
    img = nodes[0].attrib['src'].strip()
    print img
  except IndexError:
    pass

  _add2xml(link, title, brand, price, category, img, img, size, keywords, desc)

#------------------ yoyo18 ----------------
def _yoyo18(link, html):
  title=brand=price=img=size=category=keywords=desc=''
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

  _add2xml(link, title, brand, price, category, img, bigimg, size, keywords, desc)

#------------------ redbaby ----------------
def _redbaby(link, html):
  title=brand=price=img=size=category=keywords=desc=''
  html = html.decode('gb18030').replace('&nbsp;', ' ')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//div[@id='location_info02']/span")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("//span[@class='weightNumber']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//div[@class='Brand']/a")
  for node in nodes:
    brand = node.text_content().encode('utf8').strip()
    print brand 

  nodes=doc.xpath("//div[@id='location_info02']/a")
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
  _add2xml(link, title, brand, price, category, img, bigimg, size, keywords, desc)

#------------------ 7shop24 ----------------
def _7shop24(link, html):
  title=brand=price=img=size=category=keywords=desc=''
  html = html.decode('gb18030').replace('&nbsp;', ' ')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//div[@class='right_xb']/div[1]")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("//span[@class='f1 w']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//div[@class='show_sp_tp']/img")
  try:
    img = nodes[0].attrib['src']
    print img
  except IndexError:
    pass
 
  nodes=doc.xpath("//div[@class='main']/div[1]//span")
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

  _add2xml(link, title, brand, price, category, img, img, size, keywords, desc)

#------------------ lafaso ----------------
def _lafaso(link, html):
  title=brand=price=img=bigimg=size=category=keywords=desc=''
  html = html.decode('utf8').replace('&nbsp;', ' ')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//div[@class='fr shopr_boxtopR']/h1")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("//span[@class='fl mr10']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//div[@class='shopr_boxtopRcont mauto']/ul[1]/li[1]/a")
  for node in nodes:
    brand = node.text_content().encode('utf8').strip()
    print brand 

  nodes=doc.xpath("//div[@id='cont0']/div[@class='clear pa10 lh25']")
  for node in nodes:
    if(node.text_content() != ''):
      desc = node.text_content().encode('utf8').replace(' ','')
      break;

  nodes=doc.xpath("//div[@class='fl shopr_boxtopL']//img")
  try:
    img = nodes[0].attrib['src']
    print img
    bigimg = nodes[0].attrib['alt']
    print bigimg
  except IndexError:
    pass

  nodes=doc.xpath("//h2[@class='crumbs']/em")
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    if(len(str) > 12 or str.find('首页') >= 0):
      continue
    if(category == ''):
      category = str
    else:
      category = category + ',' + str
    print category 

  _add2xml(link, title, brand, price, category, img, bigimg, size, keywords, desc)

#------------------ meixiu ----------------
def _meixiu(link, html):
  title=brand=price=img=size=category=keywords=desc=''
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

  _add2xml(link, title, brand, price, category, img, img, size, keywords, desc)

#------------------ strawberrynet ----------------
def _strawberrynet(link, html):
  title=brand=price=img=size=category=keywords=desc=''
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

  nodes=doc.xpath("//div[@class='prodDetailPadding']//ul/li")
  for node in nodes:
    if(node.text_content() != ''):
      desc = node.text_content().encode('utf8').strip()
      break;

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

  _add2xml(link, title, brand, price, category, img, img, size, keywords, desc)

#------------------ m18 ----------------
def _m18(link, html):
  title=brand=price=img=bigimg=size=category=keywords=desc=''
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

  _add2xml(link, title, brand, price, category, img, img, size, keywords, desc)

#------------------ sasa ----------------
def _sasa(link, html):
  title=brand=price=img=bigimg=size=category=keywords=desc=''
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//a[@class='txt_16px_n_666666']")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("//td[@class='txt_11px_b_EB6495']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price
    if price.find('US$') >= 0:
      price = price.replace('US$','').strip()
      price = str(float(price) * 6.8)
      print price 
      break 

  nodes=doc.xpath("//a[@class='txt_16px_b_666666']")
  for node in nodes:
    brand = node.text_content().encode('utf8').strip()
    print brand 

  nodes=doc.xpath("//td[@class='txt_12px_n_666666']")
  for node in nodes:
    size = size.encode('utf8').strip()
    print size 

  nodes=doc.xpath("//td[@width='350' and @align='center']/img")
  try:
    img = nodes[0].attrib['src']
    print img
  except IndexError:
    pass

  bigimg = img.replace('n.jpg', 'l.jpg')
  _add2xml(link, title, brand, price, category, img, bigimg, size, keywords, desc)

#------------------ guopi ----------------
def _guopi(link, html):
  title=brand=price=img=bigimg=size=category=keywords=desc=''
  html = html.replace('&nbsp;', ' ')
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

  nodes=doc.xpath("//div[@class='prIntro_title']")
  for node in nodes:
    if(len(node.text_content()) > 10):
      desc += node.text_content().encode('utf8').strip() + '<br/>'

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

  _add2xml(link, title, brand, price, category, img, bigimg, size, keywords, desc)

#------------------ yihaodian ----------------
def _yihaodian(link, html):
  title=brand=price=img=bigimg=size=category=keywords=desc=''
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//span[@id='product_name']")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("//span[@id='product_sale_price' or @id='product_list_price']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//span[@id='product_format']")
  for node in nodes:
    size = node.text_content().encode('utf8').strip()
    print size 

  nodes=doc.xpath("//div[@class='pt5 pb5 gray'][1]/a[1]")
  for node in nodes:
    brand = node.text_content().encode('utf8').strip() 
  print brand 

  nodes=doc.xpath("//div[@id='picDiv']/img")
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

  _add2xml(link, title, brand, price, category, img, bigimg, size, keywords, desc)

#------------------ meethall ----------------
def _meethall(link, html):
  title=brand=price=img=size=category=keywords=desc=''
  html = html.decode('gb18030').replace('&nbsp;&nbsp;', '\t')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//div[@class='info']/ul/li[1]/strong")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("//div[@class='info']/ul/li[4]/a")
  for node in nodes:
    brand = node.text_content().encode('utf8').strip()
    xb = brand.split('\t')
    if(len(xb) >= 2):
      if(xb[0] == xb[1]):
        brand = xb[0]
    print brand

  nodes=doc.xpath("//div[@class='meethallprice']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//div[@class='detail_content']//font")
  for node in nodes:
    if(len(node.text_content()) > 10):
      desc += node.text_content().encode('utf8').strip() + '<br/>'

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

  _add2xml(link, title, brand, price, category, img, img, size, keywords, desc)

#------------------ qookoo ----------------
def _qookoo(link, html):
  title=brand=price=img=bigimg=size=category=keywords=desc=''
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//h1[@class='goodsname']")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("//span[@class='price1']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 
    break

  nodes=doc.xpath("//div[@class='goodspic']//img")
  try:
    img = nodes[0].attrib['src']
    bigimg = img
    print img
  except IndexError:
    pass

  nodes=doc.xpath("//div[@class='Navigation']/span/a")
  del nodes[0]
  del nodes[0]
  brand = nodes[0].text_content().encode('utf8').strip()
  del nodes[0]
  print brand
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    if(category == ''):
      category = str
    else:
      category = category + ',' + str
    print category

  _add2xml(link, title, brand, price, category, img, img, size, keywords, desc)

#------------------ jafei ----------------
def _jafei(link, html):
  title=brand=price=img=bigimg=size=category=keywords=desc=''
  html = html.decode('gb18030')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//span[@class='newPrice']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//div[@class='bjweb_product_n']/ul/li")
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    if(str.find('品牌') >= 0):
      brand = str.split('：')[1].split('-')[0].split('0')[0].strip()
      break
  print brand

  nodes=doc.xpath("//div[@class='bjweb_product_img']/img")
  try:
    img = nodes[0].attrib['src']
    bigimg = img
    print img
    title = nodes[0].attrib['alt']
    print title 
  except IndexError:
    pass

  nodes=doc.xpath("//div[@class='bjweb_dqwz']//a[@href][3]")
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    category = str.replace('/', ',')
    print category

  _add2xml(link, title, brand, price, category, img, bigimg, size, keywords, desc)

#------------------ cntvs ----------------
def _cntvs(link, html):
  title=brand=price=img=bigimg=size=category=keywords=desc=''
  html = html.decode('utf8')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//div[@class='starprize']/span")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//div[@id='proname']/h1")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title

  nodes=doc.xpath("//div[@class='pronumb']")
  for node in nodes:
    brand = node.text_content().encode('utf8').replace('商品品牌：','').strip()
    print brand

  nodes=doc.xpath("//div[@id='ProBox2']//img")
  try:
    img = nodes[0].attrib['src']
    bigimg = img
    print img
  except IndexError:
    pass

  nodes=doc.xpath("//div[@style]/a")
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    if(len(str) > 12 or str.find('首页') >= 0 or len(str) <= 1):
      continue
    if(category == ''):
      category = str
    else:
      category = category + ',' + str
    print category

  _add2xml(link, title, brand, price, category, img, bigimg, size, keywords, desc)

#------------------ d1 ----------------
def _d1(link, html):
  title=brand=price=img=bigimg=size=category=keywords=desc=''
  html = html.decode('gb18030')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//span[@style='color:#ff5901;']")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//meta[@name='keywords']")
  for node in nodes:
    keywords = node.attrib['content'].encode('utf8').strip()
    vs = keywords.split('-')
    title = vs[0].strip()
    brand = vs[-1].split(' ')[1].strip()
    for i in range(1, len(vs)-1):
      if category=='':
        category = vs[i]
      else:
        category = category + ',' +vs[i]
        
  nodes=doc.xpath("//div[@class='gdtltu2']//img")
  try:
    img = nodes[0].attrib['src']
    bigimg = img
    print img
  except IndexError:
    pass

  _add2xml(link, title, brand, price, category, img, bigimg, size, keywords, desc)

#------------------ u9buy ----------------
def _u9buy(link, html):
  title=brand=price=img=bigimg=size=category=keywords=desc=''
  html = html.decode('utf8')
  doc = H.document_fromstring(html)

  nodes=doc.xpath("//div[@class='starprize']/span")
  for node in nodes:
    price = node.text_content().encode('utf8').strip()
    print price 

  nodes=doc.xpath("//div[@id='proname']/h1")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title

  nodes=doc.xpath("//div[@class='pronumb']")
  for node in nodes:
    brand = node.text_content().encode('utf8').replace('商品品牌：','').strip()
    print brand

  nodes=doc.xpath("//div[@id='ProBox2']//img")
  try:
    img = nodes[0].attrib['src']
    bigimg = img
    print img
  except IndexError:
    pass

  nodes=doc.xpath("//div[@style]/a")
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    if(len(str) > 12 or str.find('首页') >= 0 or len(str) <= 1):
      continue
    if(category == ''):
      category = str
    else:
      category = category + ',' + str
    print category

  _add2xml(link, title, brand, price, category, img, bigimg, size, keywords, desc)

#------------------ yoka ----------------
def _yoka(link, html):
  title=brand=price=img=bigimg=size=category=keywords=desc=''
  html = html.decode('utf-8')
  doc = H.document_fromstring(html)

  price = '0'
  img=bigimg='http://images.yoka.com/pic/news/2009/0202/logo.jpg'

  nodes=doc.xpath("//div[@class='box1tit']/div/h1")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("//dl[@class='bnav']/dt/a[2]")
  for node in nodes:
    brand = node.text_content().encode('utf8').strip()
  print brand

  nodes=doc.xpath("//dl[@class='bnav']/dt/a[3]")
  for node in nodes:
    category = node.text_content().encode('utf8').replace('/', ',').strip()
  print category 

  nodes=doc.xpath("//div[@class='box1rg21']")
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    desc += str + '\n'
  print desc

  _add2xml(link, title, brand, price, category, img, bigimg, size, keywords, desc)

#------------------ pclady ----------------
def _pclady(link, html):
  title=brand=price=img=bigimg=size=category=keywords=desc=''
  html = html.decode('gb18030')
  doc = H.document_fromstring(html)

  price = '0'
  img=bigimg='http://www1.pclady.com.cn/global/2009/images_d/index/lady_logo.gif'

  nodes=doc.xpath("//div[@class='bigTit']")
  for node in nodes:
    title = node.text_content().encode('utf8').strip()
    print title 

  nodes=doc.xpath("//div[@class='position']/a[2]")
  for node in nodes:
    brand = node.text_content().encode('utf8').strip()
  print brand

  nodes=doc.xpath("//div[@class='position']/a[3]")
  for node in nodes:
    category = node.text_content().encode('utf8').replace('/', ',').strip()
  print category 

  nodes=doc.xpath("//div[@class='dTxt']/div[@class='pDes']")
  for node in nodes:
    str = node.text_content().encode('utf8').strip()
    desc += str + '\n'
  print desc

  _add2xml(link, title, brand, price, category, img, bigimg, size, keywords, desc)


#------------------ main ----------------
if __name__ == '__main__': 
  if(len(sys.argv) != 3):
    print 'Usage: ' + sys.argv[0] + ' <datapath> <site>'
    exit(1)
  if(sys.argv[1][-1] != '/'):
    sys.argv[1] += '/'

  global fo
  ISOTIMEFORMAT='%Y%m%d'
  timestr = time.strftime(ISOTIMEFORMAT, time.localtime())
  fo = open('/baijia/' + sys.argv[2] + '_' + timestr + '.xml', 'w')
  fd = open('/deldata/' + sys.argv[2] + '.del', 'w')

  linkdb = bsddb.btopen(sys.argv[1] + '._link.bdb', 'r')

  now = time.time()
  fo.write('<?xml version="1.0" encoding="UTF-8"?>\n<items>\n')

  i = 0
  for k, v in linkdb.iteritems():
    print '---------------------------------'
    i += 1
    print str(i) + ', ' + v
    filename = sys.argv[1] + k + '.html'
      
    if os.path.isfile(filename):
      f = open(filename, 'r')
      html = f.read()
      f.close()
    else:
      fd.write(v + '\n')
      continue

    t=eval("_" + sys.argv[2])
    try:
      t(v, html)
    except Exception, e:  
      pass

  linkdb.close()
  fo.write('</items>\n')
  fo.close()
  fd.close()

