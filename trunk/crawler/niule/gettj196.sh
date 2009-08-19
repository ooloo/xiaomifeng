rec=0
url=0

while [ 1 ]
do
	sleep 5 

	num=`wc -l /tmp/url.list | awk '{print $1}'`
	if [ $num != $url ]
	then
		url=$num
		awk '{print $2}' /tmp/url.list > seed.list
		./neoseed 10000 NORMAL seed.list
	else
		echo "no need seed."
	fi
	
	ls /home2/guangjiliang/savefile/one.* > file.list

	num=`wc -l file.list | awk '{print $1}'`

	if [ $num == $rec ]
	then 
		echo "none new file"
		continue
	fi

	rec=$num

	./trade_txt2idx file.list
done
