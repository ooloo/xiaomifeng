#!/bin/sh

export LANG=c

site=sasa
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

		./neoparse ${site}_product.xml \
		"http://www.sasa.com/SasaWeb/sch/category/listByCategory.jspa?categoryId=$1&sortBy=8&page=${page}" > /tmp/$queue

		grep "link: " /tmp/$queue | grep "viewProductDetail" | awk '{print $2}' > $queue

		python neoexplore.py $queue $explore
		if [ $? -ne 0 ]
		then
			break
		fi

		python neospider.py $queue $savedir
		sleep 2
	done
}

neorun 101000000 
neorun 102000000
neorun 103000000
neorun 104000000

