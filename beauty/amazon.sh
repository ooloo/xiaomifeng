#!/bin/sh

export LANG=c

page=0
site=amazon_queue

rm -f $site

while [ 1 ]
do
	page=$(($page+1))
	if [ $page -gt 34 ]
	then
		break
	fi

	./neoparse amazon_product.xml \
	"http://www.amazon.cn/mn/browseApp?showType=4&nodeid=61020&sortType=market&pageNow=${page}" > /tmp/$site 

	grep "link: " /tmp/$site | awk '{print $2}' >> $site

	./neoparse amazon_product.xml \
	"http://www.amazon.cn/mn/browseApp?showType=4&nodeid=51300&sortType=market&pageNow=${page}" > /tmp/$site

	grep "link: " /tmp/$site | grep detailApp | awk '{print $2}' >> $site

	sleep 2
done

sort -u $site > /tmp/$site
cp /tmp/$site $site

wget -x -N -U "Mozilla/5.0(Windows; U; Windows NT 5.1; en-US)" \
	--directory-prefix=/data/ --timeout=30 --wait=1 --random-wait -i $site 

