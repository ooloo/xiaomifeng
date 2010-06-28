#!/bin/sh

export LANG=c

site=pclady
queue=${site}_queue
savedir=/data/${site}/

mkdir -p $savedir
rm -f $queue

page=0
explore=./.explore_${site}.bdb
rm -f $explore

while [ 1 ]
do
	./neoparse pclady_product.xml \
  "http://cosme.pclady.com.cn/products_list/br0_bs0_bi0_sm0_ef0_pb0_pe0_or0_p$page.html" > /tmp/$queue

	page=$(($page+1))

	grep "/product/" /tmp/$queue| awk '{print $2}' > $queue

	python neoexplore.py $queue $explore
	if [ $? -ne 0 ]
	then
		break
	fi

	python neospider.py $queue $savedir
	sleep 2
done

