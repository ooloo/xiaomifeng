#!/bin/sh

export LANG=c

site=yihaodian
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
	"http://www.yihaodian.com/search/0-5009/k/oempty-a0-s1-p${page}" > /tmp/$queue

	grep "link: " /tmp/$queue | grep "/product/" | awk '{print $2}' > $queue

	python neoexplore.py $queue $explore
	if [ $? -ne 0 ]
	then
		break
	fi

	python neospider.py $queue $savedir
	sleep 2
done

