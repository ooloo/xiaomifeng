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
		"http://s.lafaso.com/directory/$1_0_0_0_0_00_0_000_${page}.html" > /tmp/$queue

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

neorun 19462 
neorun 19305
neorun 19408
neorun 19421
neorun 19429
neorun 19280
neorun 19301
neorun 19300
neorun 19281
neorun 19022
neorun 19021
neorun 19121
neorun 19140
neorun 19141
neorun 18984
neorun 18940
neorun 18921
neorun 18960
neorun 18961
neorun 22060

