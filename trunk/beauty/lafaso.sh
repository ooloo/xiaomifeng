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

neorun 19261
neorun 18920
neorun 19020 
neorun 19304
neorun 8300
neorun 6781
neorun 19120

