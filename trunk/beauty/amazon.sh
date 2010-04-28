#!/bin/sh

export LANG=c

site=amazon
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
	"http://www.amazon.cn/s/ref=?ie=UTF8&n=852804051&page=${page}" > /tmp/$queue

	grep "link: " /tmp/$queue | grep detailApp | awk '{print $2}' | sed 's/&qid=[0-9]\{10\}//g' > $queue
  cp $queue /tmp/$queue.$page
  
	python neoexplore.py $queue $explore
	if [ $? -ne 0 ]
	then
		break
	fi

	python neospider.py $queue $savedir
	sleep 2
done

