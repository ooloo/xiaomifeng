#!/bin/sh

mkdir -p /home3/xml/history/
mv /home3/xml/*.* /home3/xml/history/

python2.5 neoabstract.py /data/360buy 360buy
python2.5 neoabstract.py /data/amazon amazon
python2.5 neoabstract.py /data/dangdang dangdang
python2.5 neoabstract.py /data/no5 no5
python2.5 neoabstract.py /data/redbaby redbaby
python2.5 neoabstract.py /data/lafaso lafaso
python2.5 neoabstract.py /data/strawberrynet strawberrynet
python2.5 neoabstract.py /data/yihaodian yihaodian
#python2.5 neoabstract.py /data/guopi guopi
#python2.5 neoabstract.py /data/2688 2688
#python2.5 neoabstract.py /data/sasa sasa
#python2.5 neoabstract.py /data/cntvs cntvs
#python2.5 neoabstract.py /data/jafei jafei
#python2.5 neoabstract.py /data/yoyo18 yoyo18
#python2.5 neoabstract.py /data/d1 d1
#python2.5 neoabstract.py /data/7shop24 7shop24
#python2.5 neoabstract.py /data/meixiu meixiu
#python2.5 neoabstract.py /data/m18 m18
#python2.5 neoabstract.py /data/meethall meethall
#python2.5 neoabstract.py /data/qookoo qookoo
#python2.5 neoabstract.py /data/u9buy u9buy
