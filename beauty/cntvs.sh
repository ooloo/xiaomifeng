#!/bin/sh

export LANG=c

site=cntvs
queue=${site}_queue
savedir=/data/${site}/

mkdir -p $savedir
rm -f $queue

page=0
explore=./.explore_${site}.bdb
rm -f $explore

while [ 1 ]
do
	page=$(($page+1))

	./neoparse cntvs_product.xml \
	"http://app.cntvs.com/New_test/SecondNodes.aspx?id=fc2dbc81-25a1-4fb8-843f-9cc600bcf747&page=${page}" \
	> /tmp/$queue 

	grep "link: " /tmp/$queue | awk '{print $2}' > $queue

	python neoexplore.py $queue $explore
	if [ $? -ne 0 ]
	then
		break
	fi

	python neospider.py $queue $savedir
	sleep 2
done

