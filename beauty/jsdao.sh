#!/bin/sh

export LANG=c

site=jsdao
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

	./neoparse jsdao_product.xml \
	"http://www.jsdao.com/Product-10030---1-0-${page}.html" \
	> /tmp/$queue 

  grep "link: " /tmp/$queue | grep ".html" | awk '{print substr($0, 7)}' > $queue
  python neoquote.py $queue

	python neoexplore.py $queue $explore
	if [ $? -ne 0 ]
	then
		break
	fi

	python neospider.py $queue $savedir
	sleep 2
done

