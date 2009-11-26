#!/bin/sh

export LANG=c

page=0
site=2688_queue

rm -f $site

while [ 1 ]
do
	page=$(($page+1))
	if [ $page -gt 530 ]
	then
		break
	fi

	./neoparse 2688_product.xml \
	"http://www.2688.com/Search/iResult.aspx?Mt=cosmetic&Pt=cosmetic&SortType=1&listnum=30&PageNow=${page}" > /tmp/$site 

	grep "dProduct.aspx" /tmp/$site | awk '{print $2}' >> $site 

	sleep 2
done

sort -u $site > /tmp/$site
cp /tmp/$site $site

wget -x -N --directory-prefix=/data/ --timeout=30 --wait=2 -o /tmp/2688.log --random-wait -i $site 

