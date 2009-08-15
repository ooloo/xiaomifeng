#include "NetMonitorPthread.h"
#include <assert.h>

void* _monitor_thread_proc(void* param);


CNetMonitorPthread::CNetMonitorPthread(DataQueue& _addrQueue, FilterObjectQueue&_rawFilterQueue,const int Interval):
	m_addrQueue(_addrQueue),
	m_rawFilterQueue(_rawFilterQueue)

{
	m_pthread_id = 0;
	//m_thread_id = 0;
	int ret = 0;
	m_nMonitorInterval = Interval;
	ret = pthread_create(&m_pthread_id, NULL, _monitor_thread_proc, this);
	//m_hthread = CreateThread(NULL, 1024*1024, (LPTHREAD_START_ROUTINE)_monitor_thread_proc, this, 0, &m_pthread_id);
	if (m_pthread_id == 0)
	{
		fprintf(stderr, "_spider_thread_proc create thread failed.");
		assert(false);
	}
	fprintf(stdout, "CNetMonitorPthread create thread success!\n");
}

CNetMonitorPthread::~CNetMonitorPthread(void)
{
}


void* _monitor_thread_proc(void* param)
{
	if (!param)
		return 0;
	
	CNetMonitorPthread* pObj = (CNetMonitorPthread*)param; 
	//DataQueue::type_value pAddrObj = NULL; 
	CData* pAddrObj = NULL;

	CRuleMgr rulemgr;
	if (!rulemgr.loadRuleFromToFile("rule.xml"))
	{
		std::cout<<"Can't open XML file!"<<std::endl;
		return 0;
	}


	while (true )
	{
		if (pObj->m_addrQueue.empty())
			continue;
		
		//pAddrObj = pObj->m_addrQueue.front();
		fprintf(stdout, "CNetMonitorPthread starts work!\n");
		::sleep(5);
		int _size = (int)pObj->m_addrQueue.size();
		for ( int i=0; i<_size; i++)
		{
			pAddrObj = pObj->m_addrQueue.next();
			//::usleep(_nInverval);
			
			///	这个obj是一个CAddrData 对象，调用他的getData()函数就直接可以得到地址字符窜
			/// getLayer()函数可以得到网页的层次关系
			///	...从网络上抓去原始数据
			INDEX unNetlay = pAddrObj->getLayer();
			pObj->m_spider.GetHost(pAddrObj->getData().c_str());
			pObj->m_spider.AddInitNode(true);   //true表示没有树杈
			pObj->m_spider.HandleInitNode(false); //是否写文件
			
			
			/// 在数据抓去下来之后需要new CFilter对象，并将原始数据传递到CFilter对象，然后提交
			/// 给存放需要分析数据的队列
			PAGELAYER pagelayer;
			pagelayer.nCurrIndex = unNetlay;
			pagelayer.nCurrLayerIndex = unNetlay + 1;   //当前的层ID
			pagelayer.nFatherIndex = 1;
			pagelayer.sAddress = "";
			pagelayer.nRuleIndex = 0;        //没有起到作用
			pagelayer.sAddress = pObj->m_spider.GetAddrName();
			CFilter* pFilter = new CFilter(rulemgr); 
			//设置网页的层次
			if (pObj->m_spider.GetDataLenth()>0)
			{
				pFilter->put(pAddrObj);
				pFilter->put(pObj->m_spider.GetData(),(long)pObj->m_spider.GetDataLenth(),pagelayer);
                pObj->m_rawFilterQueue.push(pFilter);

				//判断是否是索引页
				if (pAddrObj->isIndexPage()&&pAddrObj->getType()==enAddrData)
			    {
				    //pObj->m_addrQueue.writeLog(pObj->m_spider.GetLogName(),pAddrObj->getData());
				    
			    }
			}
			else
			{
				std::cout<<"Can't Get html!"<<std::endl;
			}
			::usleep(pObj->m_nMonitorInterval);
		}
	}

}
