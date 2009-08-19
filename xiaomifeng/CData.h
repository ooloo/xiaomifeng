#ifndef CDATA_H_
#define CDATA_H_

#include "global.h"

////////////////////////////////////////////////////////////////
//	网页分析产生出来数据的抽象类
class CData
{
public:
	CData();
	virtual ~CData() = 0;
	
public:
	///	返回具体的数据类型
	const eDataType& getType(void) const	
	{ 
		return m_dataset.m_nType; 
	}
	void setType( const eDataType& nType );
	
	///	数据操作接口
	const std::string& getData(void) const;
	size_t length(void) const;
	void setData(const char* pszData);
	void setData(const std::string& sData);
	void setData(const DATASET& DataSet);
	void clear(void);
	
	void setIndex(INDEX nLayer, RULEINDEX nId) { m_dataset.m_nLayer = nLayer; m_dataset.m_nRIdentify = nId; }
	INDEX getLayer(void) const { return m_dataset.m_nLayer;}
	RULEINDEX getRuleIdentify(void) const { return m_dataset.m_nRIdentify; }
	void setIndexPage() { m_dataset.m_bIndexPage = true; }
	bool isIndexPage() { return m_dataset.m_bIndexPage; }
	const DATASET& getDataSet() { return m_dataset; }
	
protected:
	//eDataType m_nType;	///	数据类型,默认是一个直接存储的数据
	//std::string m_Data;	///	文本数据存储空间

	//INDEX		m_nLayer;		/// 网页层ID
	//RULEINDEX	m_nRIdentify;	///	规则ID
	//bool m_bIndexPage;          //是否是索引页
	DATASET m_dataset;
//public:
	///	数据处理函数，在数据对象产生并填入数据后需要对数据对象的数据进行处理
	///	该函数将完成该功能
	//virtual void process(void) = 0;
};

typedef CSafeQueue<CData*> DataQueue;

////////////////////////////////////////////////////////////////
//	存储数据类型，直接存储到文件或者数据库
class CSaveData : public CData
{
public:
	CSaveData(void);
	CSaveData(const char* pszData);
	CSaveData(const std::string& sData);
	CSaveData(const DATASET& dataset);
	bool ProcessData(void);
	bool ReFileName(void);
	bool MakeFileName(void);
	~CSaveData(void);
	 
private:
	bool _WriteFile(const std::string& strFileName,const char *pBuffer,std::string::size_type size);
		
private:
	static FILE* m_pFile;
	static std::string m_strFileName;
};
/////////////////////////////////////////////////////////////////
class CAddrData : public CData
{
public:
	CAddrData(void);
	CAddrData(const char*pszData);
	CAddrData(const std::string& sData);
	CAddrData(const DATASET& dataset);
	~CAddrData(void);

public:
	void setIndexPage() { CData::setIndexPage(); }	
};
//////////////////////////////////////////////////////////////////
class CLogicData: public CData
{
public:
	CLogicData(void);
	CLogicData(const char* pszData);
	CLogicData(const std::string& sData);
	~CLogicData(void);
};

////////////////////////////////////////////////////////////////
class CDataSets 
{
public:
	CDataSets(void) {}
	~CDataSets(void){}
	
	void push_back(CData* pData)
	{
		m_queue.push_back(pData);
	}
	CData* pop(void)
	{
		CData* p = NULL;
		if ( !m_queue.empty() )
		{
			p = m_queue.front();
			m_queue.pop_front();
		}
		return p;
	}
	CData* end_pop(void)
	{
		CData* p = NULL;
		if ( !m_queue.empty() )
		{
			p = m_queue.back();
			m_queue.pop_back() ;
		}
		return p;
	}
	//数据分发处理函数，主要负责根据数据的类型来分拣决定数据的流向[直接存储数据、地址数据、逻辑数据]
	void process(
			CSafeQueue<CData*>& dataQueue	///	存放需要存储到文件或者数据库的数据队列，与存储线程共享
			,CSafeQueue<CData*>& addrQueeu	///	存放下层抓取地址的队列，与数据抓取线程共享
			//,CSaveQueue<CData*>& logicQueue
			);
	
private:
	std::deque<CData*>	m_queue;
};
#endif /*CDATA_H_*/
