#ifndef CFILTER_H_
#define CFILTER_H_

#include "CData.h"
#include "CRuleMgr.h"

class CRawData;

class CFilter
{
public:
	CFilter(CRuleMgr& _mgr);
	virtual ~CFilter();
	
public:
	void put(char* _pRaw, const long nLength, const PAGELAYER& pageLayer);
	void put( const CRawData* pRawData );
	void put( CData* pData);
	
	//	处理从原始数据队列来的数据包
	void process(void);
	
	//	取出已经分析产生出来的数据集合
	CDataSets& getData(void);
	
private:
	RuleSets 	m_Rules;//规则数据集合
	std::string	m_RawData;//原始数据
	PAGELAYER	m_PageLayer;//页数据属性
	CDataSets	m_DataSet;//数据集合
	
	CRuleMgr& 	m_rulemgr;//规则管理集合

	DATASET* m_pdataset;
};

typedef CSafeQueue<CFilter*> FilterObjectQueue;
#endif /*CFILTER_H_*/
