#/bin/sh

dir=/home2/baijiapic/download/
dest=/home2/baijiapic/pic/

mkdir -p $dir
cp /home3/xml/$1.piclink $dir/
file=$dir/$1.piclink
#wget -N "http://ihot.3322.org/pic_url" -O pic_url

sort -u $file | while read link
do
	key=`echo -n $link|md5sum|cut -d' ' -f1`

	str=`expr substr "$key" 1 4`
	dir1=`printf "%02d\n" $((0x$str%32))`
	
	str=`expr substr "$key" 5 4`
	dir2=`printf "%02d\n" $((0x$str%32))`
	
	subdir=$dir1/$dir2
	mkdir -p $dest/$subdir

	if [ -s $dest/$subdir/image_150x150_$key.jpg ]
	then
		echo "$dest/$subdir/image_150x150_$key.jpg exist."
		continue;
	fi

	sleep 2
	wget --timeout=10 -U "Mozilla/5.0(Windows; U; Windows NT 5.1; en-US)" \
	--connect-timeout=5 --no-http-keep-alive --tries=1 "$link" -O $dir/$1.jpg
	#curl --connect-timeout 5 -m 10 "$link" -o $dir/$1.jpg

	/usr/local/bin/convert $dir/$1.jpg -trim /tmp/$1.a.jpg
	if [ $? -ne 0 ]
	then
		continue
	fi

	size=`/usr/local/bin/identify /tmp/$1.a.jpg | awk '{print $3}'`
	pos=`expr index $size "x"`
	len=`expr length $size`

	x=`expr substr "$size" 1 $(($pos-1))`
	y=`expr substr "$size" $(($pos+1)) $len`

	if [ $x -gt $y ]
	then
		/usr/local/bin/convert -border 0x$((($x-$y)/2)) -bordercolor white /tmp/$1.a.jpg /tmp/$1.b.jpg
	else
		/usr/local/bin/convert -border $((($y-$x)/2))x0 -bordercolor white /tmp/$1.a.jpg /tmp/$1.b.jpg
	fi

	#if [ $(($x*5)) -gt $(($y*4)) ]
	#then
	#	/usr/local/bin/convert -border 0x$((($x*5/4-$y)/2)) -bordercolor white /tmp/a.jpg /tmp/c.jpg
	#else
	#	/usr/local/bin/convert -border $((($y*4/5-$x)/2))x0 -bordercolor white /tmp/a.jpg /tmp/c.jpg
	#fi

	echo $subdir/$key.jpg
	/usr/local/bin/convert -resize 150x150! /tmp/$1.b.jpg $dest/$subdir/image_150x150_$key.jpg
	/usr/local/bin/convert -resize 200x200! /tmp/$1.b.jpg $dest/$subdir/image_200x200_$key.jpg
	#/usr/local/bin/convert -resize 240x300! /tmp/c.jpg $dest/$subdir/image_240x300_$key.jpg
done

