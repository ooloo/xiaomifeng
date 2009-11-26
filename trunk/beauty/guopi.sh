#!/bin/sh

export LANG=c

page=0
site=guopi_queue

rm -f $site

./neoparse guopi_product.xml "http://www.guopi.com/map.jsp" > /tmp/$site 

grep "blk_product" /tmp/$site | awk '{print $2}' >> $site 

sleep 2

sort -u $site > /tmp/$site
cp /tmp/$site $site

wget -x -N --directory-prefix=/data/ --timeout=30 --wait=2 -o /tmp/guopi.log --random-wait -i $site 

