#!/bin/sh

export LANG=c

site=yoyo18
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

	./neoparse ${site}_product.xml \
	"http://www.yoyo18.com/web/searchProductPage-12-0-0-0-0-0-0-0-0-0-0-0-${page}.htm" > /tmp/$queue

	grep "link: " /tmp/$queue | grep detailApp | awk '{print $2}' > $queue

	python neoexplore.py $queue $explore
	if [ $? -ne 0 ]
	then
		break
	fi

	python neospider.py $queue $savedir
	sleep 2
done

