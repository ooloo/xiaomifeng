#!/bin/sh

export LANG=c

site=amazon
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
	  "http://www.amazon.cn/s/ref=?ie=UTF8&n=$1&page=${page}" > /tmp/$queue

	  grep "link: " /tmp/$queue | grep "/dp/" | awk '{print $2}' | awk -F'/' '{if($5=="dp") print "http://"$3"/"$5"/"$6}' > $queue

	  python neoexplore.py $queue $explore
	  if [ $? -ne 0 ]
	  then
		  break
	  fi

	  python neospider.py $queue $savedir
	  sleep 2
  done
}

#neorun 852804051
neorun 746776051
