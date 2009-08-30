#!/bin/sh

wget -r -l 1 -A "p_detail.php*" \
	-N -U "Mozilla/5.0(Windows; U; Windows NT 5.1; en-US)" --directory-prefix=/data/ \
	--timeout=30 --wait=3 --random-wait http://www.gzsd38.com/ 
