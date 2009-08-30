#!/bin/sh

export LANG=c

page=0
site=loreal_queue

rm -f $site

while [ 1 ]
do
	page=$(($page+1))
	if [ $page -gt 24 ]
	then
		break
	fi

	./neoparse loreal_product.xml \
	"http://www.loreal-paris.cn/_zh/_cn/beauty-filter/index.aspx?page=${page}" > /tmp/$site 

	grep "link: " /tmp/$site | awk '{print $2}' >> $site

	sleep 2
done

sort -u $site > /tmp/$site
cp /tmp/$site $site

wget -x -U "Mozilla/5.0(Windows; U; Windows NT 5.1; en-US)" \
	--directory-prefix=/data/ --timeout=30 --wait=1 --random-wait -i $site 

