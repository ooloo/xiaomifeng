#!/bin/sh

export LANG=c

site=no5
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
		"http://www.no5.com.cn/browse/$1_p${page}.html" > /tmp/$queue

		grep "link: " /tmp/$queue | grep "/goods/" | awk '{print $2}' > $queue

		python neoexplore.py $queue $explore
		if [ $? -ne 0 ]
		then
			break
		fi

		python neospider.py $queue $savedir
		sleep 2
	done
}

neorun c1
neorun c2
neorun c3
neorun c4
neorun c5
neorun c6

