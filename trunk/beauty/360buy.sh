#!/bin/sh

export LANG=c

page=0
site=360buy_queue

rm -f $site

while [ 1 ]
do
	page=$(($page+1))
	if [ $page -gt 50 ]
	then
		break
	fi

	./neoparse 360buy_product.xml \
	"http://www.360buy.com/products/911-1125-0-0-0-0-0-0-0-0-1-1-${page}.html" > /tmp/$site

	grep "link: " /tmp/$site | grep "\/product\/" | awk '{print $2}' >> $site

	sleep 3
done

sort -u $site > /tmp/$site
cp /tmp/$site $site

wget -x -N -U "Mozilla/5.0(Windows; U; Windows NT 5.1; en-US)" \
	--directory-prefix=/data/ --timeout=30 --wait=3 --random-wait -i $site 

