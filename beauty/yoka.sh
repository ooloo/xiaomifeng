#!/bin/sh

export LANG=c

page=0
site=yoka_queue

rm -f $site

while [ 1 ]
do
	page=$(($page+1))
	if [ $page -gt 200 ]
	then
		break
	fi

	./neoparse yoka_product.xml "http://brand.yoka.com/cosmetics/all/product_0_0_0_21_all_0_${page}_100_0.htm" > /tmp/$site 

	grep "link: " /tmp/$site | awk '{print $2}' >> $site 

	sleep 3
done

sort -u $site > /tmp/$site
cp /tmp/$site $site

wget -x -N --directory-prefix=/data/ --timeout=30 --wait=3 --random-wait -i $site 

