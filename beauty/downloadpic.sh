#/bin/sh

echo $1
awk '{print $1}' $1 > tmp.$1

date
sort -u tmp.$1 > queue.$1 
rm -f tmp.$1

wget -N -nv --directory-prefix=/home2/baijiapic/download/ --timeout=15 --wait=3 --random-wait \
		-U "Mozilla/5.0(Windows; U; Windows NT 5.1; en-US)" -o /tmp/$1.log -i queue.$1 

