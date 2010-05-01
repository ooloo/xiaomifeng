#!/bin/sh
export LANG=c

site=7shop24
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

	./neoparse ${site}_product.xml \
	"http://www.7shop24.com/MakeUps/browse_MakeUps.asp?pagecode=skincare&searchtype=type&typeid=1&Page=${page}" > /tmp/$queue

	grep "link: " /tmp/$queue | grep "classid=1&productid=" | awk '{print $2}' > $queue

	python neoexplore.py $queue $explore
	if [ $? -ne 0 ]
	then
		break
	fi

	python neospider.py $queue $savedir
	sleep 2
done

