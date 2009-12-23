#!/bin/sh

export LANG=c

site=2688
queue=${site}_queue
savedir=/data/${site}/

mkdir -p $savedir
rm -f $queue

page=0
explore=./.explore_${site}.bdb
rm -f $explore

while [ 1 ]
do
	page=$(($page+1))

	./neoparse 2688_product.xml \
	"http://www.2688.com/Search/iResult.aspx?Mt=cosmetic&Pt=cosmetic&SortType=1&listnum=30&PageNow=${page}" > /tmp/$site 

	grep "dProduct.aspx" /tmp/$site | awk '{print $2}' > $site 

	python neoexplore.py $queue $explore
	if [ $? -ne 0 ]
	then
		break
	fi

	python neospider.py $queue $savedir
	sleep 2
done

