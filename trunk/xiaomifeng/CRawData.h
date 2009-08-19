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
	PAGELAYER m_dLayer;		///	层索引ID等信息描述结构体,标示该源数据的来源
	void _reset(void);
	
private:
	std::string m_RawData;
	///	内存分配器对象，可能是new出来的或者malloc分配的，或者有其他内存管理对象分配的
	CMallocMemory* m_pMemAllocate;	///	特殊内存分配器
	
	CRawData(const CRawData& cpy);
	CRawData& operator= (const CRawData& thr);
};

#endif /*CRAWDATA_H_*/
