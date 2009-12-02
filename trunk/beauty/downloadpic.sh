#/bin/sh

awk '{print $1}' $1 > tmp.$1

sort -u tmp.$1 > queue.$1
rm -f tmp.$1

mkdir -p /home2/baijiapic/download/

wget -N -nv --directory-prefix=/home2/baijiapic/download/ --timeout=15 --wait=1 --random-wait \
		-U "Mozilla/5.0(Windows; U; Windows NT 5.1; en-US)" -o /tmp/$1.log -i queue.$1

