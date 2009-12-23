//#include "stdafx.h"
#include "CRuleMgr.h"
#include "tinyxml.h"

CRule::CRule(void)
{
	m_ruleset.bDirection = true;
	m_pNewDataSet = NULL;
	m_pNextDataSet = NULL;
	m_pDataSetLink = NULL;
}
CRule::~CRule(void)
{
	
}

///	原始数据处理函数，在处理的过程中直接产生分析出来的数据，提交给CDataSets
size_t CRule::process(const std::string& strRawData, CDataSets& dataSets, const std::string& strAddr, DATASET* pData)
{
	size_t cnt =0;
	m_NextRuleSet = m_ruleset.pBrother;
	RULESET _ruleset = m_ruleset;
	m_pNewDataSet = NULL;
	m_pNextDataSet = NULL;
	m_pDataSetLink = NULL;

	if (_ruleset.bDirection)
		cnt = _findPositiveDirection(strRawData, dataSets, _ruleset, strAddr);
	else
		cnt = _findReverse(strRawData, dataSets, _ruleset, strAddr);		

	return cnt;
}

///	正向查找
size_t CRule::_findPositiveDirection(const std::string& strRawData, CDataSets& dataSets, const RULESET& stRule, const std::string& strAddr)
{
	size_t i, cnt =0, count = 0;
	std::string::size_type beginPos = 0, currPos = 0;
	std::string::size_type valBegin = 0, valEnd = 0;
	std::string _strTemp;
	const RULESET* _pRule = NULL;
	while (currPos != std::string::npos)
	{
		if ( NULL == _pRule )
		{
			count = 0;
			if (m_pDataSetLink&&m_NextRuleSet==NULL)
			{
				//_setDataSet(dataSets);
				m_pNextDataSet = NULL;
			}
			_pRule = &stRule;
		}
		else
		{
			count++;
			_setDataSet(dataSets);
			_pRule = _pRule->pBrother;
		}
		///	从beginPos位置开始查找_begin内容并返回currPos
		if (_pRule)
		{
			if (_pRule->bStatic)
			{
				std::string temp;
				temp = _pRule->strStaticData;
				cnt += _genData( temp, dataSets, _pRule, strAddr);
			}
			else
			{
				currPos = strRawData.find(_pRule->strBegin, beginPos);
				if ( currPos != std::string::npos )
				{
					valBegin = currPos+_pRule->strBegin.length();
					beginPos = valBegin;

					currPos = strRawData.find(_pRule->strEnd, valBegin);
					if ( currPos != std::string::npos )
					{
						_strTemp = strRawData.substr(valBegin, currPos-valBegin);
						cnt += _genData(_strTemp, dataSets, _pRule, strAddr);

						beginPos = currPos + _pRule->strEnd.length() - 1;
					}
					else
					{
						if(_pRule == &stRule)
							break;
						_pRule = NULL;
						for(i=0; i<count; i++)
							dataSets.end_pop();
					}
				}
				else
				{
					if(_pRule == &stRule)
						break;
					_pRule = NULL;
					for(i=0; i<count; i++)
						dataSets.end_pop();
				}
			}
		}
	}

	return cnt;
}

/// 反向查找
size_t CRule::_findReverse(const std::string& strRawData, CDataSets& dataSets, const RULESET& stRule, const std::string& strAddr)
{
	size_t cnt = 0;
	std::string::size_type beginPos = 0, currPos = 0, endPos = 0, lastEndPos = 0;
	std::string::size_type valBegin = 0, valEnd = 0;

	const RULESET* _pRule = NULL;
	std::string _strTemp;

	while (currPos != std::string::npos)
	{
		if ( NULL == _pRule )
		{
			if (m_pDataSetLink&&m_NextRuleSet==NULL)
			{
				_setDataSet(dataSets);
				m_pNextDataSet = NULL;
			}
			_pRule = &stRule;
		}
		else
			_pRule = _pRule->pChildren;
        if (_pRule)
		{
	        if (_pRule->bStatic)
			{
				std::string temp;
				temp = _pRule->strStaticData;
				cnt += _genData( temp, dataSets, _pRule, strAddr);
				if (m_pDataSetLink&&m_NextRuleSet!=NULL)
					return cnt;
			}
			else
			{
				currPos = strRawData.find(_pRule->strEnd, endPos);
				if ( currPos != std::string::npos )
				{
					endPos = currPos + stRule.strEnd.length();
					valEnd = currPos;
	
					beginPos = strRawData.rfind(_pRule->strBegin, currPos);
					if ( beginPos != std::string::npos )
					{
						valBegin = beginPos + _pRule->strBegin.length();

					    _strTemp = strRawData.substr(valBegin, currPos-valBegin);
					    cnt += _genData(_strTemp, dataSets, _pRule, strAddr);
					}
					else
					{
						_strTemp = "NULL";
						cnt += _genData(_strTemp, dataSets, _pRule, strAddr);
					}
				}
				else
				{
					_strTemp = "NULL";
					cnt += _genData(_strTemp, dataSets, _pRule, strAddr);
				}
				lastEndPos = endPos;
			}
		}
	}

	return cnt;
}
void CRule::_setDataSet(CDataSets& dataSets)
{
	CData* pData = NULL;
	if (NULL==m_pNextDataSet)
	{
		fprintf(stdout,"it cann't annalyse data!\n");
		return;
	}
	pData = new CSaveData(*m_pNextDataSet);
	dataSets.push_back(pData);
	return;
}
void CRule::_setDataSet(DATASET* pdataset)
{
	if (NULL==m_pNextDataSet)
		return;
	if (pdataset)
		m_pNextDataSet->pBrother = pdataset;

	return;
}
size_t CRule::_genData(std::string& valRaw, CDataSets& dataSets,const RULESET* stRule,const std::string& strAddr)
{

	if ( valRaw.length() <= 0 )
		return 0;
	
	std::string temp;
	CData* pData = NULL;
	m_pNewDataSet = new DATASET;
	switch (stRule->eType)
	{
	case enSaveData:
		_netLabelFiltering(valRaw);
		   
		m_pNewDataSet->m_Data = valRaw;
		m_pNewDataSet->m_nType = enSaveData;
		m_pNewDataSet->m_bIndexPage = false;
		m_pNewDataSet->m_AddrURL = strAddr;
		if (NULL == m_pNextDataSet)
	    {
		    m_pDataSetLink = m_pNewDataSet;
		    m_pNextDataSet = m_pDataSetLink;
	    }
	    else
	    {
		    m_pNextDataSet->pBrother  = m_pNewDataSet;
		    m_pNextDataSet = m_pNextDataSet->pBrother;
	    }
		if (m_pNextDataSet)
		{
			m_pNextDataSet->pBrother = NULL;
		}
		break;
	case enAddrData:
		if (stRule->strAssistantdata!="NULL")
		{
			temp = stRule->strAssistantdata;
			temp.append(valRaw.c_str());
			valRaw = temp;
		}
		m_pNewDataSet->m_Data = valRaw;
		m_pNewDataSet->m_nType = enAddrData;
		m_pNewDataSet->m_nLayer = stRule->unLayer;
		if (stRule->unLayer == 0)
			m_pNewDataSet->m_bIndexPage = true;
		else
			m_pNewDataSet->m_bIndexPage = false;
		m_pNewDataSet->m_nRIdentify = stRule->unIdentify;
		m_pNewDataSet->m_AddrURL = strAddr;
		if (NULL == m_pNextDataSet)
		{
			m_pDataSetLink = m_pNewDataSet;
			m_pNextDataSet = m_pDataSetLink;
		}
		else
		{
			m_pNextDataSet->pBrother = m_pNewDataSet;
			m_pNextDataSet = m_pNextDataSet->pBrother;
		}
		if (m_pNextDataSet)
		{
			m_pNextDataSet->pBrother = NULL;
		}
		break;
	default:
		break;
	}
	
	if (m_pNewDataSet)
	{
		m_pNewDataSet->m_nLayer = stRule->unLayer;
		m_pNewDataSet->m_nRIdentify = stRule->unIdentify;
		return 1;
	}
	return 0;
}
void CRule::_netLabelFiltering(std::string& strRawData)const
{
	std::string::size_type _nPos, _curpos = 0,_startpos = 0,_endpos = 0;

	while ((_nPos = strRawData.find_first_of("<>",_curpos)) != std::string::npos)
	{
		_curpos += 1;

		if(strRawData.compare(_nPos, 1, "<") == 0)
		{
			_startpos = _nPos;
			continue;
		}
		else if(strRawData.compare(_nPos, 1, ">") == 0)
			_endpos = _nPos + 1;

		strRawData.erase(strRawData.begin()+_startpos, strRawData.begin()+_endpos);
	}
//	fprintf(stdout,"a = %s\n",_GetData.c_str());
	_dataSpecialCharacterFiltering(strRawData);
	return;
}

//void CRule::_dataSpecialCharacterFiltering(std::string &strRawData) const
//{
//	std::string::size_type _size = strRawData.length();
//	int p = 0;
//	char *_point = NULL;
//	_point = new char[_size+1];
//	memset(_point,0,_size+1);
//	memcpy(_point,strRawData.c_str(),_size);
//	while(*(_point + p)!= 0)
//	{
//		if((*(_point + p) < 32)&&(*(_point+p)>0))
//			*(_point + p) = 32;
//		p++;
//	}
//	strRawData.clear();
//	strRawData = _point;
//	fprintf(stdout,"b = %s\n",strRawData.c_str());
//	return;
//}

//void CRule::_dataSpecialCharacterFiltering(std::string &strRawData) const
//{
//	std::string::size_type _size = strRawData.length();
//	char* _buffer = new char[_size + 1];
//	memset(_buffer,0,_size + 1);
////	const char *_point = NULL;
//	char *_point = new char[_size + 1];
//	char *_ptemp = NULL;
//	memset(_point,0,_size+1);
//	memcpy(_point,strRawData.c_str(),_size);
//	_ptemp = _point;
////	_point = strRawData.c_str();
//	std::string::size_type _start = 0,_end = 0,_curr = 0,_last = 0;
//	std::string temp;
//	while(isspace(*(strRawData.c_str() + _curr))&&*(strRawData.c_str() + _curr)!=0)
//		_curr++;
//		
//	_start = _curr;
//	while (_curr <= _size)
//	{
//		if ((*_point<32)&&(*_point>=0))
//		{
//			_start = _curr;
//			if (*_point!=0)
//				*_point = ' '; 
//		}
//		else
//		{
//			_end = _curr;
//		}
//		if ((_end == (_start - 1)) && _last < _start && _end > 0) //_end >= 0
//		{
//			memset(_buffer,0,_size + 1);
//			memcpy(_buffer, strRawData.c_str() + _last, (_end - _last) + 1);
//			temp = temp + _buffer;
//		}
//		if (_end < _start)
//		{
//			_last = _start + 1;
//		}
//			
//		_point++;
//		_curr++;
//	}
//	delete _buffer;
//	strRawData = temp;
//	fprintf(stdout,"b = %s\n",temp.c_str());
//	return;
//}

void CRule::_dataSpecialCharacterFiltering(std::string &strRawData) const
{
	std::string::size_type _size = strRawData.length();
	char* _buffer = new char[_size + 1];
	memset(_buffer,0,_size + 1);
//	const char* _point = NULL;
	char* _point = new char[_size + 1];
	memset(_point,0,_size + 1);
	memcpy(_point,strRawData.c_str(),_size);
	int _nPos = 0,_curr = 0;
	std::string temp;
//	fprintf(stdout,"begin = %s\n",strRawData.c_str());
	while (_curr <= _size)
	{	
		
		if ((*(strRawData.c_str()+_curr)>0)&&((*(strRawData.c_str()+_curr)<=32)) && _nPos==0)
		{
			_curr++;
			continue;
		}
		else if(_nPos == 0)
		{
			*(_point+_nPos++) = *(strRawData.c_str()+_curr);
//			fprintf(stdout,"aaaa = %c %d %d\n",*(_point+_nPos), _size, _nPos);
		}
		else if(_nPos>0)
		{
			if(*(strRawData.c_str()+_curr) <= 32 && ' '!=*(_point+_nPos-1) && *(strRawData.c_str()+_curr)>0)
				*(_point+_nPos++) = ' ';
			else if (*((unsigned char*)(strRawData.c_str()+_curr))>32)
			{
				*(_point+_nPos++) = *(strRawData.c_str()+_curr);
//				fprintf(stdout,"c = %c %d %d\n",*(_point+_nPos), _size, _nPos);
			}
		}
		_curr++;
	}

	*(_point+_nPos) = 0;
	
	delete _buffer;
	strRawData = _point;
//	fprintf(stdout,"end = %s\n",strRawData.c_str());
	delete _point;
	return;
}
/********************************************************************
********************************************************************/
CRuleMgr::CRuleMgr()
{
}

CRuleMgr::~CRuleMgr()
{
}

bool CRuleMgr::loadRuleFromToFile( const char* pszFileName )
{
	CRule* _rule;
    //读XML文件内容，将规则数据填充_rule
	TiXmlDocument xmlDoc;
	if (!xmlDoc.LoadFile(pszFileName))
	{
		std::cout<<"Can't open RuleXML!"<<std::endl;
		return false;
	}
	
	const TiXmlElement* pXmlClass = xmlDoc.FirstChildElement();
	const TiXmlNode* pXmlNextNode = NULL;
	const TiXmlNode* pXmlNode = NULL;
	const char* value = NULL;

	RULESET* pNewRule = NULL;
	RULESET* pNextRule = NULL;
	RULESET* pNextChildrenRule = NULL;
	RULESET* pNextBrotherRule = NULL;
	RULESET* pRuleLink = NULL;
	while (pXmlClass)
	{
		_rule = new CRule;
		value = pXmlClass->Value();
		if ( value )
			_rule->m_ruleset.strName = value;

     pXmlNextNode = pXmlClass->FirstChild();
     while(pXmlNextNode)
	 {
		
		pXmlNode = pXmlNextNode->FirstChild();
		pNewRule = new RULESET;
		while (pXmlNode)
		{
			
			value = pXmlNode->Value();
			if ( strncmp(value, "type", strlen("type")) == 0 )
			{
				value = pXmlNode->FirstChild()->Value();
				if ( strncmp(value, "data", strlen("data")) == 0 )
				{
					pNewRule->eType = enSaveData;
				}
				else if (strncmp(value, "addr", strlen("addr")) == 0 )
				{
					pNewRule->eType = enAddrData;
				}
				else if ( strncmp(value, "logic", strlen("logic")) == 0 )
				{
					pNewRule->eType = enLogicData;
				}
			}
			else if ( strncmp(value, "layer", strlen("layer")) == 0 )
			{
				pNewRule->unLayer = atoi(pXmlNode->FirstChild()->Value());
			}
			else if ( strncmp(value, "identify", strlen("identify")) == 0 )
			{
				pNewRule->unIdentify = atoi(pXmlNode->FirstChild()->Value());
			}
			else if (strncmp(value, "identify", strlen("identify")) == 0)
			{
				value = pXmlNode->FirstChild()->Value();
			}
			
			else if (strncmp(value, "begin", strlen("begin")) == 0)
			{
				pNewRule->strBegin = pXmlNode->FirstChild()->Value();
			}
			else if (strncmp(value, "end", strlen("end")) == 0)
			{
				pNewRule->strEnd = pXmlNode->FirstChild()->Value();
			}
			else if (strncmp(value, "finddirection", strlen("finddirection")) == 0)
			{
				value = pXmlNode->FirstChild()->Value();
				if ( strncmp(value, "true", strlen("true")) == 0 )
					pNewRule->bDirection = true;
				else
					pNewRule->bDirection = false;
			}
			else if (strncmp(value, "assistantdata",strlen("assistantdata")) == 0)
			{
				pNewRule->strAssistantdata = pXmlNode->FirstChild()->Value();
			}
			else if (strncmp(value, "filteringlable",strlen("filteringlable")) == 0)
			{
				value = pXmlNode->FirstChild()->Value();
				if ( strncmp(value, "true", strlen("true")) == 0 )
					pNewRule->bfiltering = true;
				else
					pNewRule->bfiltering = false;
			}
			else if (strncmp(value, "modulestype", strlen("modulestype")) == 0)
			{
				value = pXmlNode->FirstChild()->Value();
			}
			else if (strncmp(value, "static", strlen("static")) == 0)
			{
				value = pXmlNode->FirstChild()->Value();
				if ( strncmp(value, "true", strlen("true")) == 0)
					pNewRule->bStatic = true;
				else
					pNewRule->bStatic = false;
			}
			else if (strncmp(value, "data", strlen("data")) == 0)
			{
				pNewRule->strStaticData = pXmlNode->FirstChild()->Value();
			}
			else if (strncmp(value, "name", strlen("name")) == 0)
			{
				pNewRule->strName = pXmlNode->FirstChild()->Value();
			}

			pXmlNode = pXmlNode->NextSiblingElement();
		}

      if(pXmlNextNode)
	  {
		  if ( NULL == pNextRule)
	      {
			  _rule->m_ruleset = *pNewRule;
			  pNextRule = &_rule->m_ruleset;
			  pNextRule->pBrother = NULL;
			  pNextRule->pChildren = NULL;
		  }
		  else
		  {
			  if ( strncmp(value, "children", strlen("children")) == 0 )
			  {
				  if ( NULL == pNextChildrenRule )
				  {
					  pNextRule->pChildren = pNewRule;
				      pNextChildrenRule = pNextRule->pChildren;
				  }
				  else
				  {
					  pNextChildrenRule->pChildren = pNewRule;
					  pNextChildrenRule = pNextChildrenRule->pChildren;
				  }
				  
			  }
			  else
			  {
				  if ( NULL == pNextBrotherRule )
				  {
					  pNextRule->pBrother = pNewRule;
					  pNextBrotherRule = pNextRule->pBrother;
				  }
				  else
				  {
					  pNextBrotherRule->pBrother = pNewRule;
					  pNextBrotherRule = pNextBrotherRule->pBrother;
				  }
				  
			  }
			  
		  }
		  if (pNextChildrenRule)
		  {
			  pNextChildrenRule->pChildren = NULL;
			  pNextChildrenRule->pBrother = NULL;
		  }

		  if (pNextBrotherRule)
		  {
			  pNextBrotherRule->pChildren = NULL;
			  pNextBrotherRule->pBrother = NULL;
		  }
	  }
	  pXmlNextNode = pXmlNextNode->NextSiblingElement();
	  //if (pXmlNextNode)
	  //   pNewRule = new RULESET;
	  
	 }

		m_Rules.push_back(*_rule);
        pNextRule = 0;
		pXmlClass = pXmlClass->NextSiblingElement();
	}

	return true;
}

size_t CRuleMgr::insertRuleObj(const CRule& obj)
{
	m_Rules.push_back(obj);
	return m_Rules.size();
}

const CRule* CRuleMgr::getRule(const INDEX iLayer, const RULEINDEX id)const
{
	RuleSets::const_iterator it = m_Rules.begin();
	while (it != m_Rules.end())
	{
		if (it->m_ruleset.unLayer == iLayer)
		{
			if ( it->m_ruleset.unIdentify == id)
				return &(*it);
		}
		it ++;
	}
	
	return NULL;
}

size_t CRuleMgr::getRuleSets(const INDEX iLayer, RuleSets& sets)const
{
	size_t cnt = sets.size();
	RuleSets::const_iterator it = m_Rules.begin();
	while (it != m_Rules.end())
	{
		if ( it->m_ruleset.unLayer == iLayer )
			sets.push_back(*it);
		it ++;
	}
	
	return sets.size() - cnt; 
}
