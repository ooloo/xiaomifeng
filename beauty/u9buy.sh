#!/bin/sh

export LANG=c

page=0
site=u9buy_queue

rm -f $site

while [ 1 ]
do
	page=$(($page+1))
	if [ $page -gt 196 ]
	then
		break
	fi

	./neoparse u9buy_product.xml \
	"http://www.u9buy.com/class.asp?action=new&LarCode=&MidCode=&page=${page}" > /tmp/$site 

	grep "list.asp?ProdId" /tmp/$site | awk '{print $2}' >> $site 

	sleep 2
done

sort -u $site > /tmp/$site
cp /tmp/$site $site

wget -x -N --directory-prefix=/data/ --timeout=30 --wait=1 --random-wait -i $site 

