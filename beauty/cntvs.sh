#!/bin/sh

export LANG=c

page=0
site=cntvs_queue

rm -f $site

while [ 1 ]
do
	page=$(($page+1))
	if [ $page -gt 20 ]
	then
		break
	fi

	./neoparse cntvs_product.xml \
	"http://app.cntvs.com/catagory/list.aspx?id=607f0bbc-5d08-4977-a268-9b8a01043d1e&o=orders+desc&page=${page}" \
	> /tmp/$site 

	grep "link: " /tmp/$site | awk '{print $2}' >> $site 

	sleep 2
done

sort -u $site > /tmp/$site
cp /tmp/$site $site

wget -x -N --directory-prefix=/data/ --timeout=30 --wait=1 --random-wait -i $site 

