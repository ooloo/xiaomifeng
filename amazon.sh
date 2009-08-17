#!/bin/sh

page=0
site=amazon_queue

while [ 1 ]
do
	page=$(($page+1))
	if [ $page -gt 200 ]
	then
		break
	fi

	./neoparse amazon_product.xml \
	"http://www.amazon.cn/mn/browseApp?showType=4&nodeid=61020&sortType=market&pageNow=${page}" > tmp

	grep "link: " tmp | awk '{print $2}' > $site

	wget -x -N -U "Mozilla/5.0(Windows; U; Windows NT 5.1; en-US)" \
	--directory-prefix=/home/beauty/data/ --timeout=30 --wait=1 --random-wait -i $site 
	sleep 5
done
