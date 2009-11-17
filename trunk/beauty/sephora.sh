#!/bin/sh

wget -r -I productDetailAction -l 2 -A "*.html" \
	-N -U "Mozilla/5.0(Windows; U; Windows NT 5.1; en-US)" --directory-prefix=/data/ \
	--timeout=30 --wait=3 --random-wait http://www.sephora.cn/
