#!/bin/sh

export LANG=c

page=0
site=amazon_queue

rm -f $site

while [ 1 ]
do
	page=$(($page+1))
	if [ $page -gt 350 ]
	then
		break
	fi

	./neoparse amazon_product.xml \
	"http://www.amazon.cn/s/ref=?ie=UTF8&n=852804051&page=${page}" > /tmp/$site

	grep "link: " /tmp/$site | grep detailApp | awk '{print $2}' >> $site

	sleep 3
done

sort -u $site > /tmp/$site
cp /tmp/$site $site

wget -x -N -U "Mozilla/5.0(Windows; U; Windows NT 5.1; en-US)" \
	--directory-prefix=/data/ --timeout=30 --wait=3 --random-wait -i $site 

