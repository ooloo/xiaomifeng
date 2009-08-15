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
			
			///	���obj��һ��CAddrData ���󣬵�������getData()������ֱ�ӿ��Եõ���ַ�ַ���
			/// getLayer()�������Եõ���ҳ�Ĳ�ι�ϵ
			///	...��������ץȥԭʼ����
			INDEX unNetlay = pAddrObj->getLayer();
			pObj->m_spider.GetHost(pAddrObj->getData().c_str());
			pObj->m_spider.AddInitNode(true);   //true��ʾû�����
			pObj->m_spider.HandleInitNode(false); //�Ƿ�д�ļ�
			
			
			/// ������ץȥ����֮����Ҫnew CFilter���󣬲���ԭʼ���ݴ��ݵ�CFilter����Ȼ���ύ
			/// �������Ҫ�������ݵĶ���
			PAGELAYER pagelayer;
			pagelayer.nCurrIndex = unNetlay;
			pagelayer.nCurrLayerIndex = unNetlay + 1;   //��ǰ�Ĳ�ID
			pagelayer.nFatherIndex = 1;
			pagelayer.sAddress = "";
			pagelayer.nRuleIndex = 0;        //û��������
			pagelayer.sAddress = pObj->m_spider.GetAddrName();
			CFilter* pFilter = new CFilter(rulemgr); 
			//������ҳ�Ĳ��
			if (pObj->m_spider.GetDataLenth()>0)
			{
				pFilter->put(pAddrObj);
				pFilter->put(pObj->m_spider.GetData(),(long)pObj->m_spider.GetDataLenth(),pagelayer);
                pObj->m_rawFilterQueue.push(pFilter);

				//�ж��Ƿ�������ҳ
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
