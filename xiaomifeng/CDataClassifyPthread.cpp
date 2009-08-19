//#include "stdafx.h"
#include "CDataClassifyPthread.h"
#include <assert.h>

void* _pthread_classify_proc(void* param);

//////////////////////////////////////////////////////////////////////
CDataClassifyPthread::CDataClassifyPthread(FilterObjectQueue& _filterQ, DataQueue&_addrQ, DataQueue&_saveQ):
	m_filterQueue(_filterQ),
	m_addrQueue(_addrQ),
	m_saveQueue(_saveQ)
{
	m_pthread_id = 0;
	int ret = 0;
	ret = pthread_create(&m_pthread_id, NULL, _pthread_classify_proc, this);
	//m_hthread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_pthread_classify_proc, this, 0, &m_pthread_id);
	if (ret == -1)
	{
		fprintf(stderr, "_pthread_classify_proc create thread failed.\n");
		assert(false);
	}
	fprintf(stdout, "CDataClassifyPthread create thread success!\n");
}

CDataClassifyPthread::~CDataClassifyPthread()
{
	
}

void* _pthread_classify_proc(void* param)
{
	if (!param)
	{
		fprintf(stderr, "_pthread_classify_proc param==NULL\n");
		assert(false);
		return 0;
	}

	CFilter*pFilter = NULL;
	CDataSets sets;
	CDataClassifyPthread* pObj = (CDataClassifyPthread*)param;
	while (true)
	{
		pFilter = pObj->m_filterQueue.pop();
		if (!pFilter)
		{
			::sleep(1);
			continue;
		}
		
		sets = pFilter->getData();
		sets.process(pObj->m_saveQueue, pObj->m_addrQueue);
		
		delete pFilter;
		pFilter = NULL;
	}

	return 0;
}
