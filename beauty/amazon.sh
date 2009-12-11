#!/bin/sh

export LANG=c

page=0
site=amazon
queue=${site}_queue
savedir=/data/${site}/

rm -f $site

while [ 1 ]
do
	page=$(($page+1))
	if [ $page -gt 360 ]
	then
		break
	fi

	./neoparse ${site}_product.xml \
	"http://www.amazon.cn/s/ref=?ie=UTF8&n=852804051&page=${page}" > /tmp/$queue

	grep "link: " /tmp/$queue | grep detailApp | awk '{print $2}' >> $queue

	sleep 2
done

sort -u $queue > /tmp/$queue
cp /tmp/$queue $queue

python neospider.py $queue $savedir

