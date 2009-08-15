//#include "stdafx.h"
#include "CData.h"
#include <time.h>
#include <sys/time.h>

CData::CData()
{
	m_dataset.m_bIndexPage = false;
	m_dataset.pBrother = NULL;
}

CData::~CData()
{
}

void CData::setType(const eDataType& nType)
{
	m_dataset.m_nType = nType;
}

const std::string& CData::getData(void) const
{
	return m_dataset.m_Data;
}

size_t CData::length(void) const
{
	return m_dataset.m_Data.length();
}

void CData::setData(const char* pszData)
{
	if (pszData)
		m_dataset.m_Data = pszData;
}

void CData::setData(const DATASET& DataSet)
{
	if (NULL!=&DataSet)
		m_dataset = DataSet;
}
void CData::setData(const std::string& sData)
{
	m_dataset.m_Data = sData;
}

void CData::clear(void)
{
	m_dataset.m_Data.clear();
}

//////////////////////////////////////////////////////////////
std::string CSaveData::m_strFileName = "";
FILE* CSaveData::m_pFile = NULL;
CSaveData::CSaveData(void)
{
	setType(enSaveData);
	clear();
}

CSaveData::CSaveData(const char* pszData)
{
	setType(enSaveData);
	setData(pszData);
}

CSaveData::CSaveData(const std::string& sData)
{
	setType(enSaveData);
	setData(sData);
}

CSaveData::CSaveData(const DATASET& dataset)
{
	setData(dataset);
}
CSaveData::~CSaveData(void)
{

}
bool CSaveData::ProcessData()
{
	const DATASET* pCurrData = NULL;
	pCurrData = &getDataSet();
	
	int _nCount = 1;
	std::string _addrname = pCurrData->m_AddrURL;
	std::string _constant;
	_constant += _addrname;
	struct tm* _time;
	time_t _t;
	char _now[64];
	
	while (pCurrData)
	{
		_constant += "\t";
		if (pCurrData->m_Data.empty()||(pCurrData->m_Data.compare("NULL"))==0)
		{
			if (_nCount==5)
			{
				_t = time(NULL);
				_time = localtime(&_t);
				
				memset(_now,0,sizeof(_now));
				snprintf(_now,64,"%d-%02d-%02d %02d:%02d:%02d",(_time->tm_year+1900),(_time->tm_mon+1),(_time->tm_mday),(_time->tm_hour),(_time->tm_min),(_time->tm_sec));
				_constant = _constant + _now;
			}
			else
			{
				_constant = _constant + "NULL";
			}
			
		}
		else
			_constant = _constant + pCurrData->m_Data;
		pCurrData = pCurrData->pBrother;
		
		_nCount += 1;
	}

		
	if (_WriteFile(m_strFileName,_constant.c_str(),_constant.length()))
		return true;
	return false;
}
//bool CSaveData::_TableFiltering(std::string &strContent)
//{
//	std::string::size_type _size = strContent.length();
//	char* _processbuffer = new char[_size];
//	char* _point;
//	memset(_processbuffer,0,_size);
//	memcpy(_processbuffer,strContent.c_str(),_size);
//	_point = _processbuffer;
//	std::string::size_type _start = 0,_end = 0,_curr = 0;
//	std::string temp;
//	std::string temp1;
//	while (_point)
//	{
//		if (*_point==13 || *_point==10 || *_point==9)
//		{
//			_start = _curr;	
//			temp1 = strContent.substr(_start,_curr);
//		}
//		else
//		{
//			if (temp1.empty())
//				temp = strContent.substr(_start,_curr + 1);
//			else
//				temp = temp + temp1;
//		}
//		_point++;
//		_curr++;
//	}
//	strContent = _point;
//	return true;
//}
bool CSaveData::_WriteFile(const std::string& strFileName,const char *pBuffer,std::string::size_type size)
{
	fseek(m_pFile,0,SEEK_END);
	size_t _state = fwrite(pBuffer,sizeof(char),size,m_pFile);
	char _czlte = 10;
	_state = fwrite(&_czlte,sizeof(char),1,m_pFile);
	return true;
}
bool CSaveData::MakeFileName(void)
{
	char filename[256];
	struct timeval tv;
  struct tm ttm;
  gettimeofday(&tv,NULL);
  localtime_r((time_t*)&tv,&ttm);
  snprintf(filename, sizeof(filename), "%s/trade.%d.%02d.%02d.%02d.%02d.%02d", "./",
  ttm.tm_year+1900,ttm.tm_mon+1,ttm.tm_mday,ttm.tm_hour,ttm.tm_min,ttm.tm_sec);
  m_strFileName = filename;
  m_pFile = NULL;
	m_pFile = fopen(m_strFileName.c_str(),"r+");
	if (m_pFile == NULL)
		m_pFile = fopen(m_strFileName.c_str(),"w+");

	if (m_pFile == NULL)
		return false;
		
	return true;
}
bool CSaveData::ReFileName(void)
{
	std::string _strNewName = m_strFileName;
	_strNewName.insert(3,"done.");
	fclose(m_pFile);
	rename(m_strFileName.c_str(),_strNewName.c_str());
	return true;
}
///////////////////////////////////////////////////////////////
CAddrData::CAddrData(void)
{
	setType(enAddrData);
	clear();
}

CAddrData::CAddrData(const char*pszData)
{
	setType(enAddrData);
	setData(pszData);
}

CAddrData::CAddrData(const std::string& sData)
{
	setType(enAddrData);
	setData(sData);
}

CAddrData::CAddrData(const DATASET& dataset)
{
	setData(dataset);
}

CAddrData::~CAddrData(void)
{
}

/////////////////////////////////////////////////////////////////
CLogicData::CLogicData(void)
{
	setType(enLogicData);
	clear();
}

CLogicData::CLogicData(const char* pszData)
{
	setType(enLogicData);
	setData(pszData);
}

CLogicData::CLogicData(const std::string& sData)
{
	setType(enLogicData);
	setData(sData);
}

CLogicData::~CLogicData(void)
{

}

/////////////////////////////////////////////////////////
void CDataSets::process(
		CSafeQueue<CData*>& dataQueue
		,CSafeQueue<CData*>& addrQueue
		)
{
	while (!m_queue.empty())
	{
		switch(m_queue.front()->getType())
		{
			case enSaveData: 
				dataQueue.push(m_queue.front()); 
				break;
			case enAddrData: 
				addrQueue.push(m_queue.front()); 
				break;
			case enLogicData:
			    break;
			default:
			    break;
		}
		m_queue.pop_back();
	}
}
