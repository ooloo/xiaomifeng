#!/bin/sh

export LANG=c

site=d1
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
	"http://www.d1.com.cn/html/result.asp?productsortflag=0&productsort=014&spflag=1&pageno=${page}" > /tmp/$queue

	grep "link: " /tmp/$queue | grep htm | awk '{print $2}' > $queue

	python neoexplore.py $queue $explore
	if [ $? -ne 0 ]
	then
		break
	fi

	python neospider.py $queue $savedir
	sleep 2
done
