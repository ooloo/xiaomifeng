#!/bin/sh

export LANG=c

page=0
site=dangdang_queue

rm -f $site

while [ 1 ]
do
	page=$(($page+1))
	if [ $page -gt 2630 ]
	then
		break
	fi

	./neoparse dangdang_product.xml \
	"http://category.dangdang.com/list?ps=28&cat=4002074&sort=5&store=&p=${page}" > /tmp/$site

	grep "link: " /tmp/$site | grep "Product.aspx?product_id=" | awk '{print $2}' >> $site

	sleep 1
done

sort -u $site > /tmp/$site
cp /tmp/$site $site

wget -x -N -U "Mozilla/5.0(Windows; U; Windows NT 5.1; en-US)" \
	--directory-prefix=/data/ --timeout=30 --wait=1 --random-wait -o /tmp/dangdang.log -i $site 

