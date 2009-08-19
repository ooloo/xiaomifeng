//#include "stdafx.h"
#include "CDataParserPThread.h"
#include <errno.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////////////
void* _thread_proc(void* param);
///////////////////////////////////////////////////////////////////////////////////////

CDataParserPThread::CDataParserPThread(FilterObjectQueue& _putQ, FilterObjectQueue& _outQ):
	m_filterPutQ(_putQ),
	m_filterOutQ(_outQ)
{
	/*
	pthread_attr_t _arrt;
	pthread_attr_init(&_arrt);
	*/
	
	int ret = 0;
	//m_hthread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_thread_proc, this, 0, &m_pthread_id);
	ret = pthread_create(&m_id, NULL, _thread_proc, this);
	if ( ret == -1 )
	{
		fprintf(stderr, "create thread failed, %d\n", -1);
		assert(false);
	}
	fprintf(stdout, "CDataParserPThread create thread success!\n");
}

CDataParserPThread::~CDataParserPThread()
{
	
}

bool CDataParserPThread::start(void)
{
	return true;
}

bool CDataParserPThread::stop(void)
{
	return true;
}

bool CDataParserPThread::join(void** pReturn)
{
	return true;
}

void* _thread_proc(void* param)
{
	if ( !param )
	{
		assert(false);
	}
	
	CDataParserPThread* pObj = (CDataParserPThread*)param;
	CFilter* pFilter = NULL;
	
	while (true )
	{
		if ( pObj->m_filterPutQ.empty() )
		{
			::sleep(1);
			continue;
		}
		
		pFilter = pObj->m_filterPutQ.pop();
		if ( pFilter )
		{
			pFilter->process();
			pObj->m_filterOutQ.push(pFilter);
		}
	}
	
	return 0;
}
