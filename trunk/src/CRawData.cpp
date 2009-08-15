//#include "stdafx.h"
#include "CRawData.h"
#include <assert.h>

CRawData::CRawData()
{
}

CRawData::~CRawData()
{
}

void CRawData::_reset(void)
{
	m_RawData.clear();
}

void CRawData::put(const char* _ptr)
{
	if ( !_ptr )
	{
		assert(false);
		return ;
	}
	m_RawData = _ptr;
}

void CRawData::put( const char* _ptr, const PAGELAYER& _pageLayer)
{
	put(_ptr);
	setLayerInfo(_pageLayer);
}

const char* CRawData::rawptr(void) const
{
	return m_RawData.c_str();
}

long CRawData::length(void) const
{
	return m_RawData.length();
}

void CRawData::free(void)
{
}
