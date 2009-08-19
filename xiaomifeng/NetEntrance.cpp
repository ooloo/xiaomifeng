//#include "stdafx.h"
#include "NetEntrance.h"

CNetEntrance::CNetEntrance(void)
{
	m_nCrawlInterval = 3000000;
	m_nMonitorInterval = 3000000;
	m_nMonitorThread = 1;
	m_nCrawlThread = 1;
	m_nWriteFileInterval = 30;
}

CNetEntrance::~CNetEntrance(void)
{
}

bool CNetEntrance::loadSysConfig()
{
	TiXmlDocument xmlDoc;
	std::string _strFile = "SysConfig.xml";
	if (!xmlDoc.LoadFile(_strFile.c_str()))
	{
		std::cout<<"Can't open SysConfigXML!"<<std::endl;
		return false;
	}
	
	const TiXmlElement* pXmlClass = xmlDoc.FirstChildElement();
	const TiXmlNode* pXmlNode = NULL;
	const char* value = NULL;
	while (pXmlClass)
	{
		pXmlNode = pXmlClass->FirstChild();
        while(pXmlNode)
	    	{
					value = pXmlNode->Value();
					if (strncmp(value, "CrawlInterval",strlen("CrawlInterval")) == 0)
					{
						m_nCrawlInterval = atoi(pXmlNode->FirstChild()->Value());
					}
					else if (strncmp(value, "MonitorInterval",strlen("MonitorInterval")) == 0)
					{
						m_nMonitorInterval = atoi(pXmlNode->FirstChild()->Value());
					}
					else if (strncmp(value, "WriteFileInterval", strlen("WriteFileInterval")) == 0)
					{
						m_nWriteFileInterval = atoi(pXmlNode->FirstChild()->Value());
					}
					else if (strncmp(value, "CrawlThread", strlen("CrawlThread")) == 0)
					{
						m_nCrawlThread = atoi(pXmlNode->FirstChild()->Value());
					}
					else if (strncmp(value, "MonitorThread", strlen("MonitorThread")) == 0)
					{
						m_nMonitorThread = atoi(pXmlNode->FirstChild()->Value());
					}
					pXmlNode = pXmlNode->NextSiblingElement();
				}
			pXmlClass = pXmlClass->NextSiblingElement();
	}
	
	return true;
}
bool CNetEntrance::loadEntranceConfig(const std::string &strFile)
{
	TiXmlDocument xmlDoc;
		
	if (!xmlDoc.LoadFile(strFile.c_str()))
	{
		std::cout<<"Can't open RuleXML!"<<std::endl;
		return false;
	}

	std::string _strEntraceAddr;
	int _nStart = 0;
	int _nEnd = 0;
	char _num[8];
	int _nPoint = 0;
	memset(_num,0,sizeof(_num));
	std::string::size_type _Pos = 0;
	std::string::size_type _CurrPos = 0;
	if (!m_dLogFile.readText("syslog.txt"))
	{
		std::cout<<"Can't open syslog.txt"<<std::endl;
		return false;
	}
		
	int nRow = m_dLogFile.GetRowN();


	const TiXmlElement* pXmlClass = xmlDoc.FirstChildElement();
	const TiXmlNode* pXmlNode = NULL;
	const char* value = NULL;
	while (pXmlClass)
	{
		pXmlNode = pXmlClass->FirstChild();
        while(pXmlNode)
	    {
			value = pXmlNode->Value();
			if (strncmp(value, "URL",strlen("URL")) == 0)
			{
				_strEntraceAddr = pXmlNode->FirstChild()->Value();
			}
			else if (strncmp(value, "Start",strlen("Start")) == 0)
			{
				_nStart = atoi(pXmlNode->FirstChild()->Value());
			}
			else if (strncmp(value, "End", strlen("End")) == 0)
			{
				_nEnd = atoi(pXmlNode->FirstChild()->Value());
			}
			pXmlNode = pXmlNode->NextSiblingElement();
		}
		_CurrPos =  _strEntraceAddr.find("*",0);
		if (_CurrPos==std::string::npos )
		{
				m_strVectAddr.push_back(_strEntraceAddr);
				pXmlClass = pXmlClass->NextSiblingElement();
		}
		else
		{
			for (; _nStart <= _nEnd; _nStart++)
	    {
			sprintf(_num,"%d",_nStart);
			if ( _nStart > 1000)
			{
				_strEntraceAddr.replace(_CurrPos,4,_num);
			}
			else if (_nStart > 100)
			{
				_strEntraceAddr.replace(_CurrPos,3,_num);
			}else if (_nStart > 10)
			{
				_strEntraceAddr.replace(_CurrPos,2,_num);
			}else
			{
				_strEntraceAddr.replace(_CurrPos,1,_num);
			}
			for (_nPoint =0; _nPoint < nRow; _nPoint++)
			{
				if (strcmp(_strEntraceAddr.c_str(),m_dLogFile.getElement()[_nPoint][0].c_str())==0)
					break;
			}
			if (_nPoint == nRow )
				m_strVectAddr.push_back(_strEntraceAddr);
			}
			pXmlClass = pXmlClass->NextSiblingElement();
	  }
	}
	//if (!m_dConfigFile.readCSV(strFile))
	//	return false;

	
	return true;
	
	
}
const std::string CNetEntrance::getEntranceAddr()
{
	std::string _strEntranceAddr;
	std::string::size_type n = m_strVectAddr.size();
	if (!(m_strVectAddr.size()>0))
		return _strEntranceAddr;

	_strEntranceAddr = m_strVectAddr[0];
	m_strVectAddr.erase(m_strVectAddr.begin());
	return _strEntranceAddr;
}
