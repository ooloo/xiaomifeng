#!/bin/sh

export LANG=c

site=u9buy
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

		./neoparse u9buy_product.xml \
		"http://www.u9buy.com/class.asp?action=$1&LarCode=&MidCode=&page=${page}" > /tmp/$queue

		grep "list.asp?ProdId" /tmp/$queue | awk '{print $2}' > $queue 
		python neoexplore.py $queue $explore
		if [ $? -ne 0 ]
		then
			break
		fi

		python neospider.py $queue $savedir
		sleep 2
	done
}

neorun new
neorun tejia 
#neorun tuijian
#neorun hot

