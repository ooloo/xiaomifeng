#!/bin/sh
FILE=./ip_apnic
rm -f $FILE 
wget http://ftp.apnic.net/apnic/stats/apnic/delegated-apnic-latest -O $FILE
function log2() {
if [ $1 -le 1 ] 
then 
	return $pow
fi
pow=$[$pow-1]
#echo $pow $1 
log2 $[($1/2)];

return $?
}

grep 'apnic|CN|ipv4|' $FILE | cut -f 4,5 -d'|'|sed -e 's/|/ /g' | while read ip cnt
do
	#echo $ip $cnt | sed -e 's/\./ /g'
	pow=32
	log2 $cnt
	mask=$?
	echo $ip/$mask 0

#	NETNAME=`whois $ip@whois.apnic.net | sed -e '/./{H;$!d;}' -e 'x;/netnum/!d' |grep ^netname | sed -e 's/.*:      \(.*\)/\1/g' | sed -e 's/-.*//g'`
#	case $NETNAME in 
#		CHINANET|CNCGROUP)
#		echo $ip/$mask >> $NETNAME
#		;;
#		CHINATELECOM)
#		echo $ip/$mask >> $NETNAME
#		;;
#		OTHER_NETNAME_here)
#		;;
#	esac
done

grep 'apnic|TW|ipv4|' $FILE | cut -f 4,5 -d'|'|sed -e 's/|/ /g' | while read ip cnt
do
	#echo $ip $cnt "1" | sed -e 's/\./ /g'
	pow=32
	log2 $cnt
	mask=$?
	echo $ip/$mask 1

done

grep 'apnic|HK|ipv4|' $FILE | cut -f 4,5 -d'|'|sed -e 's/|/ /g' | while read ip cnt
do
	#echo $ip $cnt | sed -e 's/\./ /g'
	pow=32
	log2 $cnt
	mask=$?
	echo $ip/$mask 2

done

grep 'apnic|MO|ipv4|' $FILE | cut -f 4,5 -d'|'|sed -e 's/|/ /g' | while read ip cnt
do
	#echo $ip $cnt | sed -e 's/\./ /g'
	pow=32
	log2 $cnt
	mask=$?
	echo $ip/$mask 3

done

