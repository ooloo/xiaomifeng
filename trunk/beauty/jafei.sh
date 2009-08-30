#!/bin/sh

export LANG=c

page=0
site=jafei_queue

rm -f $site

while [ 1 ]
do
	page=$(($page+1))
	if [ $page -gt 440 ]
	then
		break
	fi

	./neoparse jafei_product.xml "http://www.jafei.com/catalog/70/index_${page}.shtml" > /tmp/$site 

	grep "link: " /tmp/$site | awk '{print $2}' >> $site 

	sleep 2
done

sort -u $site > /tmp/$site
cp /tmp/$site $site

wget -x -N --directory-prefix=/data/ --timeout=30 --wait=1 --random-wait -i $site 

