#!/bin/sh

export LANG=c

site=redbaby
queue=${site}_queue
savedir=/data/${site}/

mkdir -p $savedir
rm -f $queue

neorun()
{
	page=0
	explore=./.explore_${site}.bdb
	rm -f $explore

	while [ 1 ]
	do
		page=$(($page+1))

		./neoparse redbaby_product.xml \
		"http://binggo.suning.com/" > /tmp/$queue
		#"http://www.binggo.com/huazhuang/$1p${page}/" > /tmp/$queue

		grep "link: " /tmp/$queue | grep -E "prd_" | awk '{print $2}' > $queue

		python neoexplore.py $queue $explore
		if [ $? -ne 0 ]
		then
			break
		fi

		python neospider.py $queue $savedir
		sleep 2
	done
}

neorun a20101
