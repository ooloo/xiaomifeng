#ifndef CDATAPARSERPTHREAD_H_
#define CDATAPARSERPTHREAD_H_

#include <pthread.h>
#include "CFilter.h"

//void* _thread_proc(void* param);

class CDataParserPThread
{
public:
	CDataParserPThread(FilterObjectQueue& _putQ, FilterObjectQueue& _outQ);
	virtual ~CDataParserPThread();
	
	bool start(void);
	bool stop(void);
	bool join(void** pReturn);
	
protected:
	friend void* _thread_proc(void* param);
	
	///�����ݷ����߳�������ݺ�Ὣ�����������push���ö��У�����ֻҪ���ڲ������ݷ���ַ���OK
	FilterObjectQueue& m_filterPutQ;
	FilterObjectQueue& m_filterOutQ;
	pthread_t m_id;
	//DWORD m_pthread_id;
	//HANDLE m_hthread;
};

#endif /*CDATAPARSERPTHREAD_H_*/
