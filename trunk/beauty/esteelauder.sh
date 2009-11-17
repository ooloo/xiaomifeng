#!/bin/sh

wget -r -I /templates/products/ -l 3 -A "multiproduct.tmpl*" \
	-N -U "Mozilla/5.0(Windows; U; Windows NT 5.1; en-US)" --directory-prefix=/data/ \
	--timeout=30 --wait=2 --random-wait http://www.esteelauder.com.cn/
