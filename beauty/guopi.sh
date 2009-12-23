#!/bin/sh

export LANG=c

site=guopi
queue=${site}_queue
savedir=/data/${site}/

mkdir -p $savedir
rm -f $queue

explore=./.explore_${site}.bdb
rm -f $explore

./neoparse guopi_product.xml "http://www.guopi.com/map.jsp" > /tmp/$queue 

grep "link: " /tmp/$queue | grep "blk_product" | awk '{print $2}' > $queue 

python neoexplore.py $queue $explore
if [ $? -ne 0 ]
then
	break
fi

python neospider.py $queue $savedir


