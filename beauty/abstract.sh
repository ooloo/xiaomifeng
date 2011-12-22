#!/bin/sh

mkdir -p /home3/xml/history/
mv /home3/xml/*.xml /home3/xml/history/

/usr/local/bin/python2.5 neoabstract.py /data/360buy 360buy
/usr/local/bin/python2.5 neoabstract.py /data/amazon amazon
/usr/local/bin/python2.5 neoabstract.py /data/dangdang dangdang
/usr/local/bin/python2.5 neoabstract.py /data/no5 no5
/usr/local/bin/python2.5 neoabstract.py /data/redbaby redbaby
/usr/local/bin/python2.5 neoabstract.py /data/lafaso lafaso
/usr/local/bin/python2.5 neoabstract.py /data/strawberrynet strawberrynet
/usr/local/bin/python2.5 neoabstract.py /data/yihaodian yihaodian
#/usr/local/bin/python2.5 neoabstract.py /data/guopi guopi
#/usr/local/bin/python2.5 neoabstract.py /data/2688 2688
#/usr/local/bin/python2.5 neoabstract.py /data/sasa sasa
#/usr/local/bin/python2.5 neoabstract.py /data/cntvs cntvs
#/usr/local/bin/python2.5 neoabstract.py /data/jafei jafei
#/usr/local/bin/python2.5 neoabstract.py /data/yoyo18 yoyo18
#/usr/local/bin/python2.5 neoabstract.py /data/d1 d1
#/usr/local/bin/python2.5 neoabstract.py /data/7shop24 7shop24
#/usr/local/bin/python2.5 neoabstract.py /data/meixiu meixiu
#/usr/local/bin/python2.5 neoabstract.py /data/m18 m18
#/usr/local/bin/python2.5 neoabstract.py /data/meethall meethall
#/usr/local/bin/python2.5 neoabstract.py /data/qookoo qookoo
#/usr/local/bin/python2.5 neoabstract.py /data/u9buy u9buy
