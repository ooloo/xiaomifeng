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
	"http://www.redbaby.com.cn/Product/ProductList_$1_11${page}.htm" > /tmp/$queue

	grep "link: " /tmp/$queue | grep "/Product/ProductInfo" | awk '{print $2}' > $queue

	python neoexplore.py $queue $explore
	if [ $? -ne 0 ]
	then
		break
	fi

	python neospider.py $queue $savedir
	sleep 2
done
}

neorun 01_203_3035_80salenum_901_1096
neorun 01_203_3036_80salenum_901_1096
neorun 01_203_3037_80salenum_901_1096
neorun 01_203_3046_80salenum_901_1096
neorun 01_203_3047_80salenum_901_1096
neorun 01_203_3039_80salenum_901_1096
neorun 01_203_3040_80salenum_901_1096
neorun 01_203_30197_80salenum_901_1096
