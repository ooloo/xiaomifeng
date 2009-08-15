#ifndef CNETSITANALYSE_H_
#define CNETSITANALYSE_H_

#include "predefine.h"
#include <errno.h>


#ifdef WIN32_LEAN_AND_MEAN	
    #include <WinSock2.h>
#else
    #include <sys/socket.h>
    #include <netdb.h>
    #include <netinet/in.h>
#endif

#include "dReader.h"

class CNetSitAnalyse
{
public:
	CNetSitAnalyse();
	virtual ~CNetSitAnalyse();

	
public:
    //��������������
	int GetHost(const std::string& src);
	//
	void AddInitNode(bool bLink);
	void HandleInitNode(bool bWriteFile);
	int GetDataLenth()
	{
		return m_nSize;
	}
	char* GetData()
	{
		return m_BufferPool;
	}
	const std::string& GetLogName()
	{
		return m_strLogName;
	}
	const std::string& GetAddrName()
	{
		return m_strAddrName;
	}
	
	
private:
	bool ConnectWeb(void);
	void Rstrchr(char * sSrc, int x, char ** sPos);
	bool HandOneNode(bool bWriteFile);
	int GetLocalAgent(char* sUserAgent, 
			          char* sAccept, 
			          char* sAcceptLanguage, 
			          char* sAcceptEncoding, 
			          char* sAcceptCharset, 
			          char* sKeepAlive, 
			          char* sConnection, 
			          char* sContentType);
	void DisplayNode();
	bool DoneWithList(bool bWriteFile);
	bool DoOnce(bool bWriteFile);
	void SendRequest(void);
	bool ReceiveResponse(bool bWriteFile);
	bool WriteFile(const char* sSrc);
	int GetNetDataLengh();
	
	
private:
	struct sockaddr_in m_server_addr;
	int m_nSockfd, m_nDsend, m_nTotalsend, m_nBytes, m_nReqn,m_nRet;
	struct hostent* m_pHost;
	char m_Request[102401], m_Buffer[1024*10+1], m_Httpheader[3073];
	char *m_BufferPool;
	WEBNODE* m_pNodeCurr;
	char* mapped_mem;
	int m_nFileNumber;
	std::string m_outputpath;
	std::string m_sOutFileType;
	char m_UserAgent[215], m_Accept[215], m_AcceptLanguage[215], m_AcceptEncoding[1024], m_AcceptCharset[215], m_KeepAlive[215], m_Connection[215], m_ContentType[215];
	char m_sWeb[100],m_sFile[100],m_sDir[100];
	char m_nPort;
	int m_nSize;
	bool m_bBrother;
	u_int m_nError;
	std::string m_strLogName;
	std::string m_strAddrName;
};

#endif /*CNETSITANALYSE_H_*/
