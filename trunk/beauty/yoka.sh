#!/bin/sh

export LANG=c

site=yoka
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
  "http://brand.yoka.com/cosmetics/all/product_0_0_0_21_all_0_${page}_100_0.htm" > /tmp/$queue

	grep "cosmetics" /tmp/$queue| grep "detail" | awk '{print $2}' > $queue

	python neoexplore.py $queue $explore
	if [ $? -ne 0 ]
	then
		break
	fi

	python neospider.py $queue $savedir
	sleep 2
done

