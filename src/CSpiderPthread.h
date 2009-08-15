#ifndef CSPIDERPTHREAD_H_
#define CSPIDERPTHREAD_H_

#include <pthread.h>
#include "CFilter.h"
#include "CNetSitAnalyse.h"
#include "tradebdb.h"

class CSpiderPthread
{
public:
	CSpiderPthread(DataQueue& _addrQueue, FilterObjectQueue&_rawFilterQueue,const int nInterval);
	virtual ~CSpiderPthread();
	
protected:
	friend void* _spider_thread_proc(void* param);
	
	FilterObjectQueue& m_rawFilterQueue;
	DataQueue& m_addrQueue;

	pthread_t m_thread_id;
	//DWORD m_pthread_id;
	//HANDLE m_hthread;
	int m_nCrawlInterval;
	CNetSitAnalyse m_spider;
};

#endif /*CSPIDERPTHREAD_H_*/
