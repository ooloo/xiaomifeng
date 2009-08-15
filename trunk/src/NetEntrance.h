#ifndef CNETENTRANCE_H
#define CNETENTRANCE_H

#include "dReader.h"
#include "tinyxml.h"

class CNetEntrance
{
	typedef std::vector<std::string> tStringArrayAddr;
	typedef std::vector<std::string>::iterator tStringAddr;
public:
	CNetEntrance(void);
	virtual ~CNetEntrance(void);

public:
	bool loadEntranceConfig(const std::string& strFile);
	const std::string getEntranceAddr();
	const int getCrawlInterval() { return m_nCrawlInterval; };
	const int getMonitorInterval() { return m_nMonitorInterval; };
	const int getMonitorThread() { return m_nMonitorThread; };
	const int getCrawlThread() { return m_nCrawlThread; };
	const int getWriteFileInterval() { return m_nWriteFileInterval; };
	bool loadSysConfig();
	
private:
	tStringArrayAddr m_strVectAddr;
	tStringArrayAddr m_strVectAddrLog;
	dFileReader m_dConfigFile;
	dFileReader m_dLogFile;
	
	int m_nCrawlInterval;
	int m_nMonitorInterval;
	int m_nMonitorThread;
	int m_nCrawlThread;
	int m_nWriteFileInterval;
};
#endif
