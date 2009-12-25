#!/bin/sh

export LANG=c

site=lafaso
queue=${site}_queue
savedir=/data/${site}/

mkdir -p $savedir
rm -f $queue

explore=./.explore_${site}.bdb

neorun()
{
	page=0
	rm -f $explore

	while [ 1 ]
	do
		page=$(($page+1))

		./neoparse ${site}_product.xml \
		"http://www.lafaso.com/directory/list.html?dire=$1&pageNo=${page}" > /tmp/$queue

		grep "link: " /tmp/$queue | grep "/product/" | awk '{print $2}' > $queue

		python neoexplore.py $queue $explore
		if [ $? -ne 0 ]
		then
			break
		fi

		python neospider.py $queue $savedir
		sleep 2
	done
}

neorun 10420
neorun 10440
neorun 10680
neorun 10562
neorun 8720
neorun 10460
neorun 10560
neorun 10280

