#!/bin/sh

export LANG=c

page=0
site=2688_queue

rm -f $site

while [ 1 ]
do
	page=$(($page+1))
	if [ $page -gt 311 ]
	then
		break
	fi

	./neoparse 2688_product.xml \
	"http://www.2688.com/Search/iResult.aspx?Mt=COSMETIC&Pt=COSMETIC&SortType=1&PageNow=${page}" > /tmp/$site 

	grep "dProduct.aspx" /tmp/$site | awk '{print $2}' >> $site 

	sleep 2
done

sort -u $site > /tmp/$site
cp /tmp/$site $site

wget -x -N --directory-prefix=/data/ --timeout=30 --wait=1 --random-wait -i $site 

