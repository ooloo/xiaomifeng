#ifndef CDATAPROCESS_H
#define CDATAPROCESS_H

#include "global.h"
#include "CData.h"
#include <pthread.h>



class CDataProcessThread
{
public:
	CDataProcessThread(DataQueue& _saveQueue,const int Interval);
	virtual ~CDataProcessThread(void);

protected:
	friend void* _pthread_dataprocess_pro(void* param);

	DataQueue& m_saveQueue;
	pthread_t m_pthread_id;
	int m_nWriteFileInterval;
	//HANDLE m_hthread;
	//DWORD m_pthread_id;
};
#endif
