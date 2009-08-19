#!/bin/sh

wget -r -I goods -l 2 -A "*.html" \
	-N -U "Mozilla/5.0(Windows; U; Windows NT 5.1; en-US)" --directory-prefix=/home/beauty/data/ \
	--timeout=30 --wait=3 --random-wait http://www.no5.com.cn/
