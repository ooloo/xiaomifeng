#!/bin/sh

export LANG=c

site=qookoo
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
		"http://www.qookoo.com/category.php?id=$1&page=${page}.html" > /tmp/$queue

		grep "link: " /tmp/$queue | grep "goods.php?id=" | awk '{print $2}' > $queue

		python neoexplore.py $queue $explore
		if [ $? -ne 0 ]
		then
			break
		fi

		python neospider.py $queue $savedir
		sleep 2
	done
}

neorun 80
neorun 136
neorun 79
neorun 75
neorun 42

