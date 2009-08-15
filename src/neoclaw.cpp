// NetLeah.cpp : Defines the entry point for the console application.
//

#include "predefine.h"
#include "CNetSitAnalyse.h"

#include "CDataParserPThread.h"
#include "CSpiderPthread.h"
#include "CDataClassifyPthread.h"
#include "NetEntrance.h"
#include "DataProcessThread.h"
#include "NetMonitorPthread.h"
#include "tradebdb.h"

int main()
{
	int _nCrawlInterval = 0;
	int _nMonitorInterval = 0;
	int _nWriteFileInterval = 0;
	int _nCrawlThread = 0;
	int _nMonitorThread = 0;
	
	CNetEntrance _EntranceRule;
	DataQueue Entranceaddress_queue, mysql_data_queue,address_queue;
	std::string _EntranceAddr;
		
	if (!_EntranceRule.loadSysConfig())
		return 0;
			
	if (!_EntranceRule.loadEntranceConfig("EntranceConfig.xml"))
		return 0;
		
		
	_nCrawlInterval = _EntranceRule.getCrawlInterval();
	_nMonitorInterval = _EntranceRule.getMonitorInterval();
	_nWriteFileInterval = _EntranceRule.getWriteFileInterval();
	_nCrawlThread = _EntranceRule.getCrawlThread();
	_nMonitorThread = _EntranceRule.getMonitorThread();
	
	
	_EntranceAddr = _EntranceRule.getEntranceAddr();
	while (!_EntranceAddr.empty())
	{
		CAddrData* addrdata = new CAddrData(_EntranceAddr);
	    addrdata->setIndex(0, 1);
		addrdata->setIndexPage();
	    Entranceaddress_queue.push(addrdata);
		_EntranceAddr = _EntranceRule.getEntranceAddr();
	}
	if (!Entranceaddress_queue.empty())
		fprintf(stdout, "EntranceAddr's init success!\n");
	else
	{
		fprintf(stdout, "EntranceAddr's init false!\n");
		return 0;
	}

	//sAddr = "http://search.china.alibaba.com/selloffer/%E7%A4%BC%E5%93%81/10.html";
	//addrdata = new CAddrData(sAddr);
	//addrdata->setIndex(0, 1);
	//address_queue.push(addrdata);
    
	FilterObjectQueue filter_put_queue, filter_out_queue;
	neo_bdb_init();

	CDataParserPThread pthread_parser_obj(filter_put_queue, filter_out_queue);

	CDataClassifyPthread pthread_classify_obj(filter_out_queue, address_queue, mysql_data_queue);

	CNetMonitorPthread pthread_monitor_obj1(Entranceaddress_queue,filter_put_queue,_nMonitorInterval);
	//CSpiderPthread pthread_spider_obj1(Entranceaddress_queue, filter_put_queue,_nCrawlInterval);
	CSpiderPthread pthread_spider_obj1(address_queue, filter_put_queue,_nCrawlInterval);

	//CSpiderPthread pthread_spider_obj2(address_queue, filter_put_queue);

	//CSpiderPthread pthread_spider_obj3(address_queue, filter_put_queue);

	//CSpiderPthread pthread_spider_obj4(address_queue, filter_put_queue);
    
	CDataProcessThread oo(mysql_data_queue,_nWriteFileInterval);
	while (true)
        {
		      ::sleep(60);
		      neo_bdb_sync();
		      if (Entranceaddress_queue.empty()&&filter_put_queue.empty()&&filter_out_queue.empty()&&address_queue.empty()&&mysql_data_queue.empty())
							break;
        }

	std::cout<<"NetLeah has finished!"<<std::endl;
	return 0;
}

