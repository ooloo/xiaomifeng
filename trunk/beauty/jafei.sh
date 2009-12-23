#!/bin/sh

export LANG=c

site=jafei
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

		./neoparse jafei_product.xml \
		"http://www.jafei.com/catalog/$1/index_${page}.shtml" > /tmp/$queue 

		grep "link: " /tmp/$queue | awk '{print $2}' > $queue 

		python neoexplore.py $queue $explore
		if [ $? -ne 0 ]
		then
			break
		fi

		python neospider.py $queue $savedir
		sleep 2
	done
}

neorun 70
neorun 71
neorun 72
neorun 73
neorun 74
neorun 75
neorun 76

