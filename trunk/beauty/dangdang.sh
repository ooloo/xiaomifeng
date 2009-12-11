#!/bin/sh

export LANG=c

page=0
site=dangdang
queue=${site}_queue
savedir=/data/${site}/

rm -f $site

while [ 1 ]
do
	page=$(($page+1))
	if [ $page -gt 130 ]
	then
		break
	fi

	./neoparse dangdang_product.xml \
	"http://category.dangdang.com/list?ps=28&cat=4002074&sort=5&store=eq0&p=${page}" > /tmp/$queue

	grep "link: " /tmp/$queue | grep "Product.aspx?product_id=" | awk '{print $2}' >> $queue

	sleep 1
done

sort -u $queue > /tmp/$queue
cp /tmp/$queue $queue

python neospider.py $queue $savedir

