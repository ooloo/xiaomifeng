#!/bin/sh

export LANG=c

page=0
site=redbaby_queue

rm -f $site

while [ 1 ]
do
	page=$(($page+1))
	if [ $page -gt 160 ]
	then
		break
	fi

	./neoparse redbaby_product.xml \
	"http://www.redbaby.com.cn/Product/Product_List.aspx?&Site=01&BranchID=3&Page=${page}" > /tmp/$site

	grep "link: " /tmp/$site | grep "/Product/ProductInfo" | awk '{print $2}' >> $site

	sleep 2
done

sort -u $site > /tmp/$site
cp /tmp/$site $site

wget -x -N -U "Mozilla/5.0(Windows; U; Windows NT 5.1; en-US)" \
	--directory-prefix=/data/ -o /tmp/redbaby.log --timeout=30 --wait=2 --random-wait -i $site 

