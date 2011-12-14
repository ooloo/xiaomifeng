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
	"http://www.binggo.com/huazhuang/$1p${page}/" > /tmp/$queue

	grep "link: " /tmp/$queue | grep -E "/huazhuang/[0-9]+\.html" | awk '{print $2}' > $queue

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
neorun a20102 
neorun a20103
neorun a20104
neorun a20105
neorun a20106
neorun a20107
neorun a20108
