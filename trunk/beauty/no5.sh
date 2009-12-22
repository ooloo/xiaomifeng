#!/bin/sh

export LANG=c

page=0
site=no5
queue=${site}_queue
savedir=/data/${site}/

mkdir -p $savedir
rm -f $queue

while [ 1 ]
do
	page=$(($page+1))

	./neoparse ${site}_product.xml \
	"http://www.no5.com.cn/browse/c1_p${page}.html" > /tmp/$queue

	grep "link: " /tmp/$queue | grep "\/goods\/" | awk '{print $2}' > $queue

	python neoexplore.py $queue $savedir
	if [ $? -ne 0 ]
	then
		break
	fi

	python neospider.py $queue $savedir

	sleep 2
done

