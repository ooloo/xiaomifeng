edit:	neocharset.o neoparse.o neoclaw.o NetEntrance.o CDataParserPThread.o CSpiderPthread.o NetMonitorPthread.o \
	CDataClassifyPthread.o CDataParserPThread.o DataProcessThread.o CNetSitAnalyse.o \
	CFilter.o CRawData.o CRuleMgr.o tinystr.o safe-ctype.o utils.o URL.o \
	tinyxmlerror.o tinyxmlparser.o tinyxml.o dReader.o CData.o tradebdb.o

	g++ -o neoclaw -g -lpthread -ldb -lssl neoclaw.o NetEntrance.o CDataClassifyPthread.o \
	CDataParserPThread.o NetMonitorPthread.o \
	CSpiderPthread.o tradebdb.o CNetSitAnalyse.o DataProcessThread.o CFilter.o CRawData.o \
	CRuleMgr.o tinystr.o tinyxmlerror.o tinyxmlparser.o tinyxml.o dReader.o CData.o

	g++ -o neoparse -g neoparse.o safe-ctype.o utils.o URL.o CFilter.o CRawData.o \
	CRuleMgr.o tinystr.o tinyxmlerror.o tinyxmlparser.o tinyxml.o CData.o -I./ -lssl ./libcharsetdetector.a

	g++ -o neocharset -g neocharset.o -I./ ./libcharsetdetector.a
all:	$(clear) $(edit)
#Exe:	g++ -o neoclaw -g neoclaw.o CDataClassifyPthread.o CDataParserPThread.o \
#	CSpiderPthread.o CNetSitAnalyse.o CNetDataSet.o \
#	CFilter.o CRawData.o CRuleMgr.o tinystr.o tinyxmlerror.o tinyxmlparser.o tinyxml.o dExcelReader.o CData.o
	
neoparse.o:	neoparse.cpp
	g++ -c -g neoparse.cpp
neoclaw.o:	neoclaw.cpp
	g++ -c -g neoclaw.cpp
utils.o:	utils.cpp
	g++ -c -g utils.cpp
URL.o:	URL.cpp
	g++ -c -g URL.cpp
safe-ctype.o:	safe-ctype.cpp
	g++ -c -g safe-ctype.cpp
NetEntrance.o:	NetEntrance.cpp
	g++ -c -g NetEntrance.cpp
CDataParserPThread.o:	CDataParserPThread.cpp
	g++ -c -g CDataParserPThread.cpp
NetMonitorPthread.o: NetMonitorPthread.cpp
	g++ -c -g NetMonitorPthread.cpp
CSpiderPthread.o:	CSpiderPthread.cpp tradebdb.cpp
	g++ -c -g CSpiderPthread.cpp
CDataClassifyPthread.o:	CDataClassifyPthread.cpp
	g++ -c -g CDataClassifyPthread.cpp
DataProcessThread.o:	DataProcessThread.cpp
	g++ -c -g DataProcessThread.cpp
CNetSitAnalyse.o:	CNetSitAnalyse.cpp
	g++ -c -g CNetSitAnalyse.cpp
CFilter.o:	CFilter.cpp
	g++ -c -g CFilter.cpp
CRawData.o:	CRawData.cpp
	g++ -c -g CRawData.cpp
CRuleMgr.o:	CRuleMgr.cpp
	g++ -c -g CRuleMgr.cpp
tinystr.o:	tinystr.cpp
	g++ -c -g tinystr.cpp
tinyxmlerror.o:	tinyxmlerror.cpp
	g++ -c -g tinyxmlerror.cpp
tinyxmlparser.o:	tinyxmlparser.cpp
	g++ -c -g tinyxmlparser.cpp
tinyxml.o: tinyxml.cpp
	g++ -c -g tinyxml.cpp
dReader.o:	dReader.cpp
	g++ -c -g dReader.cpp
CData.o:	CData.cpp
	g++ -c -g CData.cpp
tradebdb.o:	tradebdb.cpp
	g++ -c -g tradebdb.cpp
neocharset.o:	neocharset.cpp
	g++ -c -g neocharset.cpp

clean:
	rm -f *.o
