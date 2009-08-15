#ifndef CRAWDATA_H_
#define CRAWDATA_H_

#include "global.h"

class CRawData
{
public:
	CRawData();
	virtual ~CRawData();
	
	void put(const char* _ptr );
	void put(const char* _ptr, const PAGELAYER& _pageLayer );
	
	const char* rawptr(void) const;
	long length(void) const;
	
	void free(void);
	
	void setLayerInfo( const PAGELAYER& pageLayer) { m_dLayer = pageLayer; }
	const PAGELAYER& layerInfo(void) const { return m_dLayer; }
	
protected:
	PAGELAYER m_dLayer;		///	������ID����Ϣ�����ṹ��,��ʾ��Դ���ݵ���Դ
	void _reset(void);
	
private:
	std::string m_RawData;
	///	�ڴ���������󣬿�����new�����Ļ���malloc����ģ������������ڴ�����������
	CMallocMemory* m_pMemAllocate;	///	�����ڴ������
	
	CRawData(const CRawData& cpy);
	CRawData& operator= (const CRawData& thr);
};

#endif /*CRAWDATA_H_*/
