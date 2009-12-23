#!/bin/sh

export LANG=c

site=360buy
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

	./neoparse 360buy_product.xml \
	"http://www.360buy.com/products/911-1125-0-0-0-0-0-0-0-0-1-1-${page}.html" > /tmp/$queue

	grep "link: " /tmp/$queue | grep "/product/" | awk '{print $2}' > $queue

	python neoexplore.py $queue $explore
	if [ $? -ne 0 ]
	then
		break
	fi

	python neospider.py $queue $savedir
	sleep 2
done

