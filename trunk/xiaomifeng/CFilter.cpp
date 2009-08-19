//#include "stdafx.h"
#include "CFilter.h"
#include "CRawData.h"
#include <assert.h>

CFilter::CFilter(CRuleMgr& _mgr):m_rulemgr(_mgr)
{
	m_pdataset = NULL;
}

CFilter::~CFilter()
{
}

void CFilter::put(char* _pRaw, const long nLength, const PAGELAYER& pageLayer)
{
	if ( !_pRaw && nLength <= 0 )
	{
		assert(false);
		return ;
	}
	
	m_RawData.clear();
	_pRaw[nLength - 1] = '\0';
	m_RawData = _pRaw;
	m_PageLayer = pageLayer;
	
	m_Rules.clear();
	m_rulemgr.getRuleSets(m_PageLayer.nCurrLayerIndex, m_Rules);
}

void CFilter::put( const CRawData* pRawData )
{
	if ( !pRawData )
	{
		assert(false);
		return ;
	}
	
	if ( pRawData->length() <= 0 )
	{
		assert(false);
		return ;
	}
	
	m_RawData.clear();
	pRawData->rawptr();
	m_RawData = pRawData->rawptr();
	m_PageLayer = pRawData->layerInfo();
	
	m_Rules.clear();
	m_rulemgr.getRuleSets(m_PageLayer.nCurrLayerIndex, m_Rules);
}
void CFilter::put(CData* pData)
{
	if (pData->getDataSet().pBrother)
	{
		m_pdataset = pData->getDataSet().pBrother;
	}
	return;
}
void CFilter::process(void)
{
	RuleSets::iterator rule_it = m_Rules.begin();
	while (rule_it != m_Rules.end())
	{
		rule_it->process(m_RawData, m_DataSet,m_PageLayer.sAddress,m_pdataset);
		rule_it ++;
	}
}

CDataSets& CFilter::getData(void)
{
	return m_DataSet;
}

