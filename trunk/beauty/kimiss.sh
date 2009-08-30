#!/bin/sh

wget -r -I product -l 3 -A "*.html" -D "product.kimiss.com"\
	-N -U "Mozilla/5.0(Windows; U; Windows NT 5.1; en-US)" --directory-prefix=/data/ \
	--timeout=30 --wait=3 --random-wait http://product.kimiss.com/

rm -rf /data/www.kimiss.com/
