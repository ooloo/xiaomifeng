#ifndef GLOBAL_H_
#define GLOBAL_H_
#include "predefine.h"
#include <string>
#include <queue>
#include <list>
#include <stdio.h>
#include "CMallocMemory.h"

/*		��ҳ���������������
 *
 *	    ����Ԫ�أ�
 *		��ϵ��ҳ������id��		///	��ϵ��ҳ�Ĳ��ID�ǵ�ǰ���ID-1
 *		��ҳ����������id��
 *		ĳһ����ҳ��������
 *		��ҳ����ַ��
 *		��ҳ��������id��
 */

typedef unsigned long INDEX;
typedef unsigned long RULEINDEX;

typedef struct stPAGELAYER
{
	INDEX nFatherIndex;
	INDEX nCurrLayerIndex;
	INDEX nCurrIndex;
	std::string sAddress;
	RULEINDEX nRuleIndex;
}
PAGELAYER, *PPAGELAYER;


/*	��ҳ���������
 * 	
 * �����ķ�����Ԫ�����������ȡ
 * ͨ���Ի�����Ԫ���ݵ����������ʵ�ָ�Ϊ���ӵ��߼���������
 */


/* 	������������������˵���������ͷ�Ϊ3��
 * 1��������Ŀ�����ݣ����Դ洢�ڽ��ʻ��߷�������
 * 2���������յ����ݣ�ֻ��һ����ҳ�ĵ�ַ������һ����Ҫ��������ҳ��ַ
 * 3������һ�������ǰ�����ֶ�Ҫ����һЩ���������ͣ�����һ���߼��������ͣ�
 * ���Ȳ���ʾʵ�ʵ�����Ҳ����ʾ��ҳ�ĵ�ַ�����Ǳ�ʾһ���߼����
 */
typedef enum  eDataType
{
	enSaveData = 1,		///	������������
	enAddrData,		///	��ַ��������
	enLogicData,	/// �߼���������	
};
/*
  //���򼯺ϣ���ȡ�����еĸ����������ݣ�ͨ������ĵ���֯���淶���ݵĸ�ʽ
  //
*/
typedef struct stRULE
{
	eDataType eType;
	INDEX unLayer;
	RULEINDEX unIdentify;
	bool bDirection;
	std::string strBegin;
	std::string strEnd;
	std::string strName;
	std::string strAssistantdata;
	struct stRULE* pBrother;
	struct stRULE* pChildren;
	bool bfiltering;
	bool bStatic;
	std::string strStaticData;
}
RULESET,*PRULESET;

typedef struct stDATA
{
	eDataType m_nType;	///	��������,Ĭ����һ��ֱ�Ӵ洢������
	std::string m_Data;	///	�ı����ݴ洢�ռ�

	INDEX		m_nLayer;		///��ҳ��ID
	RULEINDEX	m_nRIdentify;	///����ID
	bool m_bIndexPage;          ///�Ƿ�������ҳ
	std::string m_AddrURL;      ///URL��ַ
	struct stDATA* pBrother;    ///����������Ϊbrother���͵�����,��ʼ��ʶ�������ʶ����ҳ�������Ψһ��
	//struct stDATA* pChildren;   ///����������Ϊchildren���͵����ݣ��ǽ����ڵ�ַ������
}
DATASET,*PDATASET;
//////////////////////////////////////////////////////
///	�Զ���
class CAutoLock
{
public:
	CAutoLock(pthread_mutex_t& _mutex):_lock(_mutex)
	{
		pthread_mutex_lock(&_lock);
	}
	~CAutoLock(void)
	{
		pthread_mutex_unlock(&_lock);
	}
private:
	pthread_mutex_t& _lock;

	CAutoLock(const CAutoLock& cpy);
	CAutoLock& operator= (const CAutoLock& thr);
};
/////////////////////////////////////////////////////
//	���ݶ���
template<typename T>
class CSafeQueue
{
public:
	CSafeQueue(void)
	{
		m_nCurrPoint = 0;
		pthread_mutex_init(&m_hLock,NULL);
	}
	~CSafeQueue(void)
	{
		pthread_mutex_destroy(&m_hLock);
	}
	
	typedef T type_value;
public:
	size_t size(void)
	{
		CAutoLock __a_lock(m_hLock);
		return m_list.size();
	}
	bool empty(void)
	{
		CAutoLock __a_lock(m_hLock);
		return m_list.empty();
	}
	T pop(void)
	{
		T _t = NULL;
		CAutoLock __a_lock(m_hLock);
		if ( m_list.empty() )
			return _t;

		_t =  m_list.front();
		m_list.pop_front();
		return _t;
	}
	T front(void)
	{
		T _t = NULL;
		CAutoLock __a_lock(m_hLock);
		if ( m_list.empty() )
			return _t;

		_t = m_list.front();
		return _t;
	}
	void push(const T& t)
	{
		CAutoLock __a_lock(m_hLock);
		m_list.push_back(t);
	}
	T end(void)
	{
		T _t = NULL;
		CAutoLock __a_lock(m_hLock);
		if ( m_list.empty() )
			return _t;

		_t = *m_list.end();
		return _t;
	}
	T next(void)
	{
		m_listmember = m_list.begin();
		for (int i=0; i!=m_nCurrPoint&&m_listmember!=m_list.end(); i++)
			m_listmember++;

		if (m_listmember!=m_list.end())
			m_nCurrPoint++;
		else
		{
			m_nCurrPoint = 0;
			m_listmember = m_list.begin();
		}

		return *m_listmember;
	}
bool writeLog(const std::string& strFile,const std::string& strBuffer)
	{
		CAutoLock __a_lock(m_hLock);
		FILE* _pLocalf = NULL;
		if (strFile.empty()||strBuffer.empty())
			return false;
		_pLocalf = fopen(strFile.c_str(),"r+");
		if (_pLocalf == NULL)
				_pLocalf = fopen(strFile.c_str(),"w+");
				
		int nSize = (int)strBuffer.length();
	  char* buffer = new char[nSize];
	  memcpy(buffer,strBuffer.c_str(),nSize);
		fseek(_pLocalf,0,SEEK_END);
		size_t state = fwrite(buffer,sizeof(char),nSize,_pLocalf);
		char szIte = 10;
	    state = fwrite(&szIte,sizeof(char),1,_pLocalf);
		fclose(_pLocalf);
		return true;
	}
	
private:
	pthread_mutex_t  m_hLock;
	std::list<T>	m_list;
	int m_nCurrPoint;
	typename std::list<T>::iterator m_listmember;
private:
	CSafeQueue& operator= (const CSafeQueue& thr);
	CSafeQueue(const CSafeQueue& cpy);
};
#endif /*GLOBAL_H_*/
