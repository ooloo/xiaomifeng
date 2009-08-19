#ifndef CDATACLASSIFYPTHREAD_H_
#define CDATACLASSIFYPTHREAD_H_

#include "global.h"
#include "CData.h"
#include "CFilter.h"
#include <pthread.h>

class CDataClassifyPthread
{
public:
	CDataClassifyPthread(FilterObjectQueue& _filterQ, DataQueue&_addrQ, DataQueue&_saveQ);
	virtual ~CDataClassifyPthread();
	
protected:
	friend void* _pthread_classify_proc(void* param);
	
	
	FilterObjectQueue& m_filterQueue;
	DataQueue& m_addrQueue, &m_saveQueue;
	pthread_t m_pthread_id;
	//HANDLE m_hthread;
	//DWORD m_pthread_id;
};

#endif /*CDATACLASSIFYPTHREAD_H_*/
