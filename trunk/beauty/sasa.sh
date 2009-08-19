#!/bin/sh

page=0
site=sasa_queue

while [ 1 ]
do
	page=$(($page+1))
	if [ $page -gt 260 ]
	then
		break
	fi

	./neoparse sasa_product.xml \
	"http://www.sasa.com/SasaWeb/sch/category/listByCategory.jspa?categoryId=101000000&sortBy=8&page=${page}" > tmp

	grep "link: " tmp | awk '{print $2}' > $site 

	wget -x -N --directory-prefix=/home/beauty/data/ --timeout=30 --wait=1 --random-wait -i $site 
	sleep 5
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
	"http://www.sasa.com/SasaWeb/sch/category/listByCategory.jspa?categoryId=102000000&sortBy=8&page=${page}" > tmp

	grep "link: " tmp | awk '{print $2}' > $site 

	wget -x -N --directory-prefix=/home/beauty/data/ --timeout=30 --wait=1 --random-wait -i $site 
	sleep 5
done

