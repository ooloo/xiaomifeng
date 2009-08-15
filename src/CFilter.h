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
	
	//	�����ԭʼ���ݶ����������ݰ�
	void process(void);
	
	//	ȡ���Ѿ������������������ݼ���
	CDataSets& getData(void);
	
private:
	RuleSets 	m_Rules;//�������ݼ���
	std::string	m_RawData;//ԭʼ����
	PAGELAYER	m_PageLayer;//ҳ��������
	CDataSets	m_DataSet;//���ݼ���
	
	CRuleMgr& 	m_rulemgr;//���������

	DATASET* m_pdataset;
};

typedef CSafeQueue<CFilter*> FilterObjectQueue;
#endif /*CFILTER_H_*/
