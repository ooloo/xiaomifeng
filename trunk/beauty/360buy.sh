#!/bin/sh

export LANG=c

site=360buy
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
	  "http://www.360buy.com/products/911-1125-$1-0-0-0-0-0-0-0-1-5-${page}.html" > /tmp/$queue

	  grep "link: " /tmp/$queue | grep "/product/" | awk '{print $2}' > $queue

	  python neoexplore.py $queue $explore
	  if [ $? -ne 0 ]
	  then
		  break
	  fi

	  python neospider.py $queue $savedir
	  sleep 2
  done
}

neorun 1139
neorun 1146
neorun 1140
neorun 1138
neorun 1134
neorun 1149
neorun 1150
neorun 1128
neorun 1129
neorun 1142
neorun 1127
neorun 1143
neorun 1141
neorun 1130

