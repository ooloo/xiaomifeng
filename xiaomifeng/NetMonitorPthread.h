#ifndef NetMonitorPthread_H
#define NetMonitorPthread_H

#include "CFilter.h"
#include "CNetSitAnalyse.h"
#include <pthread.h>

class CNetMonitorPthread
{
public:
	CNetMonitorPthread(DataQueue& _addrQueue, FilterObjectQueue&_rawFilterQueue,const int Interval);
	~CNetMonitorPthread(void);


protected:
	friend void* _monitor_thread_proc(void* param);
	
	FilterObjectQueue& m_rawFilterQueue;
	DataQueue& m_addrQueue;
	pthread_t m_pthread_id;
	int m_nMonitorInterval;
	CNetSitAnalyse m_spider;

};
#endif
