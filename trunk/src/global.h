#ifndef GLOBAL_H_
#define GLOBAL_H_
#include "predefine.h"
#include <string>
#include <queue>
#include <list>
#include <stdio.h>
#include "CMallocMemory.h"

/*		网页层次索引规则描述
 *
 *	    基本元素：
 *		父系网页的索引id、		///	父系网页的层次ID是当前层次ID-1
 *		网页层次索引层次id、
 *		某一层网页的索引、
 *		网页的网址、
 *		网页分析规则id、
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


/*	网页分析规则库
 * 	
 * 基本的分析单元：标记数据提取
 * 通过对基本单元数据的组合运用来实现更为复杂的逻辑分析功能
 */


/* 	数据类型描述，这里说的数据类型分为3种
 * 1。是最后的目的数据，可以存储在介质或者服务器中
 * 2。不是最终的数据，只是一个网页的地址，是下一个需要分析的网页地址
 * 3。这是一种相对于前面两种都要特殊一些的数据类型，它是一个逻辑数据类型，
 * 它既不表示实际的数据也不表示网页的地址，而是表示一个逻辑结果
 */
typedef enum  eDataType
{
	enSaveData = 1,		///	最终数据类型
	enAddrData,		///	地址数据类型
	enLogicData,	/// 逻辑数据类型	
};
/*
  //规则集合，提取规则中的各种属性数据，通过规则的的组织来规范数据的格式
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
	eDataType m_nType;	///	数据类型,默认是一个直接存储的数据
	std::string m_Data;	///	文本数据存储空间

	INDEX		m_nLayer;		///网页层ID
	RULEINDEX	m_nRIdentify;	///规则ID
	bool m_bIndexPage;          ///是否是索引页
	std::string m_AddrURL;      ///URL地址
	struct stDATA* pBrother;    ///规则中类型为brother类型的数据,开始标识或结束标识在网页里必须是唯一的
	//struct stDATA* pChildren;   ///规则中类型为children类型的数据，是紧跟在地址的数据
}
DATASET,*PDATASET;
//////////////////////////////////////////////////////
///	自动锁
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
//	数据队列
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
