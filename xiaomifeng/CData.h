#ifndef CDATA_H_
#define CDATA_H_

#include "global.h"

////////////////////////////////////////////////////////////////
//	��ҳ���������������ݵĳ�����
class CData
{
public:
	CData();
	virtual ~CData() = 0;
	
public:
	///	���ؾ������������
	const eDataType& getType(void) const	
	{ 
		return m_dataset.m_nType; 
	}
	void setType( const eDataType& nType );
	
	///	���ݲ����ӿ�
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
	//eDataType m_nType;	///	��������,Ĭ����һ��ֱ�Ӵ洢������
	//std::string m_Data;	///	�ı����ݴ洢�ռ�

	//INDEX		m_nLayer;		/// ��ҳ��ID
	//RULEINDEX	m_nRIdentify;	///	����ID
	//bool m_bIndexPage;          //�Ƿ�������ҳ
	DATASET m_dataset;
//public:
	///	���ݴ������������ݶ���������������ݺ���Ҫ�����ݶ�������ݽ��д���
	///	�ú�������ɸù���
	//virtual void process(void) = 0;
};

typedef CSafeQueue<CData*> DataQueue;

////////////////////////////////////////////////////////////////
//	�洢�������ͣ�ֱ�Ӵ洢���ļ��������ݿ�
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
	//���ݷַ�����������Ҫ����������ݵ��������ּ�������ݵ�����[ֱ�Ӵ洢���ݡ���ַ���ݡ��߼�����]
	void process(
			CSafeQueue<CData*>& dataQueue	///	�����Ҫ�洢���ļ��������ݿ�����ݶ��У���洢�̹߳���
			,CSafeQueue<CData*>& addrQueeu	///	����²�ץȡ��ַ�Ķ��У�������ץȡ�̹߳���
			//,CSaveQueue<CData*>& logicQueue
			);
	
private:
	std::deque<CData*>	m_queue;
};
#endif /*CDATA_H_*/
