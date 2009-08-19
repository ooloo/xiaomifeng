//#include "stdafx.h"
#include "DataProcessThread.h"
#include <errno.h>
#include <assert.h>

void* _pthread_dataprocess_pro(void* param);

CDataProcessThread::CDataProcessThread(DataQueue& _saveQueue,const int Interval):
                    m_saveQueue(_saveQueue)
{
	m_pthread_id = 0;
	//m_hthread = 0;
	int ret = 0;
	m_nWriteFileInterval = Interval;
	ret = pthread_create(&m_pthread_id, NULL, _pthread_dataprocess_pro, this);


	//m_hthread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_pthread_dataprocess_pro,this,0,&m_pthread_id);
	if ( ret == -1 )
	{
		fprintf(stderr, "_pthread_dataprocess_proc create thread failed.\n");
		assert(false);
	}
	fprintf(stdout, "CDataProcessThread create thread success!\n");
}

CDataProcessThread::~CDataProcessThread(void)
{
}

void* _pthread_dataprocess_pro(void* param)
{
  CSaveData* pSaveData = NULL;
	CDataProcessThread* pObj = (CDataProcessThread*)param;
	bool _bWriteFile = false;
	while (!pSaveData)
	{
		pSaveData = (CSaveData*)pObj->m_saveQueue.pop();
		if (pSaveData)
			pSaveData->MakeFileName();
			
		while (pSaveData)
		{
			if (pSaveData->ProcessData())
		 		_bWriteFile = true;
		 	
			delete pSaveData;
		
			pSaveData = (CSaveData*)pObj->m_saveQueue.pop();
		}
		
		if (_bWriteFile)
		{
			pSaveData->ReFileName();
			_bWriteFile = false;
		}
		::sleep(pObj->m_nWriteFileInterval);
			
	}
	return 0;
}
