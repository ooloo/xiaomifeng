#!/bin/sh

page=0
site=yoka_queue

while [ 1 ]
do
	page=$(($page+1))
	if [ $page -gt 200 ]
	then
		break
	fi

	./neoparse yoka_product.xml "http://brand.yoka.com/cosmetics/all/product_0_0_0_21_all_0_${page}_100_0.htm" > tmp

	grep "link: " tmp | awk '{print $2}' > $site 

	wget -x -N --directory-prefix=/home/beauty/data/ --timeout=30 --wait=1 --random-wait -i $site 
	sleep 5
done
