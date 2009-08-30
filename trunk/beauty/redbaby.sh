#!/bin/sh

wget -r -I Product -l 2 -A "ProductInfo_*.htm*" \
	-N -U "Mozilla/5.0(Windows; U; Windows NT 5.1; en-US)" --directory-prefix=/data/ \
	--timeout=30 --wait=3 --random-wait http://www.redbaby.com.cn/ 
