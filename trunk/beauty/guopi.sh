#!/bin/sh

export LANG=c

site=guopi
queue=${site}_queue
savedir=/data/${site}/

mkdir -p $savedir
rm -f $queue

page=0
explore=./.explore_${site}.bdb
rm -f $explore

while [ 1 ]
do
	./neoparse guopi_product.xml \
	"http://www.guopi.com/browse.jsp?mp=blk_browse&i_start=$page" > /tmp/$queue 

	page=$(($page+20))

	grep "link: " /tmp/$queue | grep "browse.jsp?mp=blk_product" | awk '{print $2}' > $queue

	python neoexplore.py $queue $explore
	if [ $? -ne 0 ]
	then
		break
	fi

	python neospider.py $queue $savedir
	sleep 2
done

