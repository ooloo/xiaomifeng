#!/bin/sh

export LANG=c

page=0
site=sasa_queue

rm -f $site

while [ 1 ]
do
	page=$(($page+1))
	if [ $page -gt 260 ]
	then
		break
	fi

	./neoparse sasa_product.xml \
	"http://www.sasa.com/SasaWeb/sch/category/listByCategory.jspa?categoryId=101000000&sortBy=8&page=${page}" > /tmp/$site

	grep "link: " /tmp/$site | awk '{print $2}' >> $site 

	sleep 2
done

page=0

while [ 1 ]
do
	page=$(($page+1))
	if [ $page -gt 20 ]
	then
		break
	fi

	./neoparse sasa_product.xml \
	"http://www.sasa.com/SasaWeb/sch/category/listByCategory.jspa?categoryId=102000000&sortBy=8&page=${page}" > /tmp/$site

	grep "link: " /tmp/$site | awk '{print $2}' >> $site 

	sleep 2
done

sort -u $site > /tmp/$site
cp /tmp/$site $site

wget -x -N --directory-prefix=/data/ --timeout=30 --wait=1 --random-wait -i $site 

