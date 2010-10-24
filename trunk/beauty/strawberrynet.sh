#!/bin/sh

export LANG=c

site=strawberrynet
queue=${site}_queue
savedir=/data/${site}/

mkdir -p $savedir
rm -f $queue

explore=./.explore_${site}.bdb

neorun()
{
	rm -f $explore

	./neoparse ${site}_product.xml \
	"http://cn.strawberrynet.com/$1/" > /tmp/$queue

	grep "link: " /tmp/$queue | awk '{print $2}' > $queue.index

	cat $queue.index | while read link
	do
		./neoparse ${site}_product.xml $link > /tmp/$queue
		sleep 1

		grep "link: " /tmp/$queue | grep -E "/[0-9]+/$" | awk '{print $2}' > $queue

		python neoexplore.py $queue $explore
		if [ $? -ne 0 ]
		then
			break
		fi

		python neospider.py $queue $savedir
		sleep 3
	done
}

neorun skincare
neorun makeup
neorun haircare
neorun perfume
neorun cologne

