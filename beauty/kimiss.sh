#!/bin/sh

export LANG=c

site=kimiss
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
		"http://brand.kimiss.com/brand/$1/p/${page}/" > /tmp/$queue

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

neorun 1000

