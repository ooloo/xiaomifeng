#!/bin/sh

/sbin/pidof python
if [ $? -eq 0 ]
then
  echo "found once, plz stop it."
  exit
fi

rm /tmp/run.log

./360buy.sh >> /tmp/run.log &
./amazon.sh >> /tmp/run.log &
./redbaby.sh >> /tmp/run.log &
./dangdang.sh >> /tmp/run.log &
./lafaso.sh >> /tmp/run.log &
#./yihaodian.sh >> /tmp/run.log &
#./jumei.sh >> /tmp/run.log &
#./strawberrynet.sh >> /tmp/run.log &
#./no5.sh >> /tmp/run.log &
#./jafei.sh >> /tmp/run.log &
#./guopi.sh >> /tmp/run.log &
#./sasa.sh >> /tmp/run.log &
#./cntvs.sh >> /tmp/run.log &
#./yoyo18.sh >> /tmp/run.log &
#./d1.sh >> /tmp/run.log &
#./meixiu.sh >> /tmp/run.log &
#./m18.sh >> /tmp/run.log &
#./meethall.sh >> /tmp/run.log &
#./qookoo.sh >> /tmp/run.log &
#./2688.sh >> /tmp/run.log &
#./u9buy.sh >> /tmp/run.log &
#./yoka.sh >> /tmp/run.log &
#./pclady.sh >> /tmp/run.log &

