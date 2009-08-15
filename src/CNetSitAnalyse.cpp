//#include "stdafx.h"
#include "CNetSitAnalyse.h"

CNetSitAnalyse::CNetSitAnalyse()
{
	memset(&m_server_addr, 0,sizeof(m_server_addr));
	m_BufferPool = new char[716801];
	m_nSockfd = 0;
	m_nDsend = 0;
	m_nTotalsend = 0;
	m_nBytes = 0;
	m_nReqn = 0;
	m_nRet = 0;
	m_nFileNumber = 0;
	m_pNodeCurr = 0;
	m_outputpath = "";//m_sysconfig.getElement()[1][0];
	m_sOutFileType = ".html";//m_sysconfig.getElement()[1][1];
	m_nError = 0;
	
	memset(m_sWeb,0,sizeof(m_sWeb));
	memset(m_sFile,0,sizeof(m_sFile));
	memset(m_sDir,0,sizeof(m_sDir));
	memset(m_BufferPool,0,sizeof(m_BufferPool));
	
	memset(m_UserAgent,0,sizeof(m_UserAgent));
	memset(m_Accept,0,sizeof(m_Accept));
	memset(m_AcceptLanguage,0,sizeof(m_AcceptLanguage));
	memset(m_AcceptEncoding,0,sizeof(m_AcceptEncoding));
	memset(m_AcceptCharset,0,sizeof(m_AcceptCharset));
	memset(m_KeepAlive,0,sizeof(m_KeepAlive));
	memset(m_Connection,0,sizeof(m_Connection));
	memset(m_ContentType,0,sizeof(m_ContentType));

	//WORD wVersionRequested;
	//WSADATA wsaData;
	//wVersionRequested = MAKEWORD(1,1);
	//int err = WSAStartup( wVersionRequested, &wsaData );
	m_strLogName = "syslog.txt";
}

CNetSitAnalyse::~CNetSitAnalyse()
{
	delete [] m_BufferPool;
}
/**************************************************************
功能：连接一个网站服务器
***************************************************************/
bool CNetSitAnalyse::ConnectWeb(void)
{
	 /* connect to web server */
	  /* create a socket descriptor */
	  if((m_nSockfd=socket(PF_INET,SOCK_STREAM,0))==-1)
	  {
	    fprintf(stderr,"\tSocket Error:%s\a\n",strerror(errno));
	    return false;
	  }

	  /* bind address */
	  
	  m_server_addr.sin_family = AF_INET;
	  m_server_addr.sin_port = htons(m_pNodeCurr->port);
	  m_server_addr.sin_addr = *((struct in_addr *)m_pHost->h_addr);

	  /* connect to the server */
	  if(connect(m_nSockfd, (struct sockaddr *)(&m_server_addr), (int)sizeof(struct sockaddr)) == -1)
	  {
	    fprintf(stderr, "\tConnect Error:%s\a\n", strerror(errno));
	    return false;
	  }
	return true;
}
/**************************************************************
功能：在字符串 sSrc 里搜索 x 字符，并设置指针 sPos 指向该位置
***************************************************************/
void CNetSitAnalyse::Rstrchr(char * sSrc, int x, char ** sPos)
{
	int len = strlen(sSrc) - 1;
	while(len >= 0)	{
		if(x == sSrc[len]) 
		{
			//memcpy(*sPos,sSrc + len,1);
			(*sPos) = sSrc + len; 
			return;
		}
		len--;
	}
	(*sPos) = 0;
}
/**************************************************************
功能：从字符串 src 中分析出网站地址和端口，并得到文件和目录
***************************************************************/
int CNetSitAnalyse::GetHost(const std::string& src)
{
	char* pC,*pD,*pB;
	if (src.empty())
		return -1;

	m_strAddrName = src;
	char* pA = new char[src.length()];
	memset(pA,0,src.length());
	int len;
	m_nPort = 0;
	
	if(!strncmp(src.c_str(), "http://", strlen("http://")))
	    memcpy(pA,src.c_str()+strlen("http://"),src.length()-strlen("http://"));
	  /* else if(!strncmp(pA, "https://", strlen("https://")))  pA = src+strlen("https://"); */
	else 
		return -1;
	pB = strchr(pA, '/');
	  //pB = strchr(pA, '/');

if(pB)  
{
	len = src.length()-strlen("http://") - strlen(pB);
	//len = pA.length() - pB.length();
	memset(m_sWeb,0,sizeof(m_sWeb));
	memcpy(m_sWeb,pA,len);
    if(*(pB+1))  
    {
	    Rstrchr(pB + 1, '/', &pC);
	    if(pC) 
			 len = (strlen(pB) + 1) - strlen(pC);
	    else 
		     len = 0;
	    if(len > 0) 
	    {
	         memset(m_sDir,0,sizeof(m_sDir));
	         memcpy(m_sDir,pB + 1,len - 2);

	         if(pC + 1) 
	         {
	            len = strlen(pC + 1);
	            memset(m_sFile,0,sizeof(m_sFile));
	            memcpy(m_sFile,pC + 1,len);
	         }
	         else 
	         {
	            len = 1;
	            memset(m_sFile,0,sizeof(m_sFile));
	            memcpy(m_sFile,SINGE,len);
	         }
	    }
	    else 
	    {
	          len = 1;
	          memset(m_sDir,0,sizeof(m_sDir));
	          memcpy(m_sDir,SINGE + 1,len);

	          len = strlen(pB) + 1;
	          memset(m_sFile,0,sizeof(m_sFile));
	          memcpy(m_sFile,pB + 1,len);
	    }
	}
    else 
    {
	    len = 1;
	    memset(m_sDir,0,sizeof(m_sDir));
	    memcpy(m_sDir,SINGE + 1,len);

	    len = 1;
	    memset(m_sFile,0,sizeof(m_sFile));
	    memcpy(m_sFile,SINGE,len);
    }
}
else 
{
	
	len = src.length();
	memset(m_sWeb,0,sizeof(m_sWeb));
	memcpy(m_sWeb,pA,src.length());
	len = 1;
	memset(m_sDir,0,sizeof(m_sDir));
	memcpy(m_sDir,SINGE + 1,len);
	len = 1;
	memset(m_sFile,0,sizeof(m_sFile));
	memcpy(m_sFile,SINGE,len);
}

pD = strchr(m_sWeb,':');
if(pD)  
	m_nPort = atoi(pD + 1);
else 
    m_nPort = 80;

return 0;
}
/**************************************************************
功能：为根节点设置兄弟节点
***************************************************************/
void CNetSitAnalyse::AddInitNode(bool bLink)
{
	
	char filename[MAXFILENAME + 1] = "";
    m_bBrother = bLink;
	if(bLink)
	{
		if(m_pNodeCurr!=NULL)
		{
			m_pNodeCurr->brother = 0;
			m_pNodeCurr->child = 0;
			m_pNodeCurr->host = m_sWeb;
			m_pNodeCurr->page = m_sFile;
			m_pNodeCurr->dir = m_sDir;
			sprintf(filename, "file%05d", m_nFileNumber++);
			m_outputpath.append(filename);
			m_outputpath.append(m_sOutFileType);
			m_pNodeCurr->file = m_outputpath;
			m_pNodeCurr->port = m_nPort;
			m_pNodeCurr->IsHandled = 0;
			m_pNodeCurr->nSize = 0;
		}
		else
		{
			m_pNodeCurr = new webnode;
			m_pNodeCurr->brother = 0;
			m_pNodeCurr->child = 0;
			m_pNodeCurr->host = m_sWeb;
			m_pNodeCurr->page = m_sFile;
			m_pNodeCurr->dir = m_sDir;
			sprintf(filename, "file%05d", m_nFileNumber++);
			m_outputpath.append(filename);
			m_outputpath.append(m_sOutFileType);
			m_pNodeCurr->file = m_outputpath;
			m_pNodeCurr->port = m_nPort;
		    m_pNodeCurr->IsHandled = 0;
		    m_pNodeCurr->nSize = 0;
		}
	}
	else
	{
		WEBNODE* NewNode;
		NewNode = new webnode;
	    if (m_pNodeCurr == NULL)
		{
			NewNode->brother = 0;
			NewNode->child = 0;
		    m_pNodeCurr = NewNode;
		}
		else
		{
			NewNode->brother = 0;
			NewNode->child = 0;
			m_pNodeCurr->brother = NewNode;
		}
			
			
		NewNode->host = m_sWeb;
		NewNode->page = m_sFile;
		NewNode->dir = m_sDir;
		sprintf(filename, "file%05d", m_nFileNumber++);
		m_outputpath.append(filename);
		m_outputpath.append(m_sOutFileType);
	    NewNode->file = m_outputpath;
		NewNode->port = m_nPort;
		NewNode->IsHandled = 0;
		NewNode->nSize = 0;
	}
	
	
}
/**************************************************************
功能：处理根节点信息
***************************************************************/
void CNetSitAnalyse::HandleInitNode(bool bWriteFile)
{
	if (m_bBrother)
	{
		if(m_pNodeCurr)	
		{
			if (m_pNodeCurr->IsHandled == 0)
			{
				if (HandOneNode(bWriteFile))
					{
						if(DEBUG)	
						{
						    DisplayNode(); /* display every node */
							fprintf(stdout,"\n");
						}
					}
			}
		}

	}
	else
	{
		if(m_pNodeCurr)	
		{
			while(m_pNodeCurr)
			{
				if(m_pNodeCurr->IsHandled == 0)	
				{
					if (HandOneNode(bWriteFile))
					{
						if(DEBUG)	
						{
						    DisplayNode(); /* display every node */
							fprintf(stdout,"\n");
						}
					}					
				}
				//if(m_pNodeCurr->child && m_pNodeCurr->child->IsHandled == 0)	
				//{
				//	HandleInitNode(m_pNodeCurr->child);
				//}
				    m_pNodeCurr = m_pNodeCurr->brother;
			}
		}
	}
	return;
}

/**************************************************************
功能：处理单个节点信息
***************************************************************/
bool CNetSitAnalyse::HandOneNode(bool bWriteFile)
{
	memset(m_UserAgent,0,sizeof(m_UserAgent));
	memset(m_Accept,0,sizeof(m_Accept));
	memset(m_AcceptLanguage,0,sizeof(m_AcceptLanguage));
	memset(m_AcceptEncoding,0,sizeof(m_AcceptEncoding));
	memset(m_AcceptCharset,0,sizeof(m_AcceptCharset));
	memset(m_KeepAlive,0,sizeof(m_KeepAlive));
	memset(m_Connection,0,sizeof(m_Connection));
	memset(m_ContentType,0,sizeof(m_ContentType));

	
	if(NULL==(m_pHost=gethostbyname(m_pNodeCurr->host.c_str()))) /* get ip address by domain */
	{  
		fprintf(stderr,"\tGethostname '%s' error, %s\n", m_pNodeCurr->host.c_str(), strerror(errno));
		if (DEBUG)
		    m_nError = m_nError + 1;

		return false;
	}
	GetLocalAgent(m_UserAgent, 
			      m_Accept, 
			      m_AcceptLanguage, 
			      m_AcceptEncoding, 
			      m_AcceptCharset, 
			      m_KeepAlive, 
			      m_Connection, 
			      m_ContentType); /* Get client browser information */

	if(strcmp(m_pNodeCurr->dir.c_str(), "/"))	
		sprintf(m_Request, "GET /%s/%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\nAccept: %s\r\nConnection: %s\r\n\r\n", m_pNodeCurr->dir.c_str(), strcmp(m_pNodeCurr->page.c_str(), "@")?m_pNodeCurr->page.c_str():"", m_pNodeCurr->host.c_str(), m_UserAgent, m_Accept, m_Connection);
	else	
		sprintf(m_Request, "GET %s%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\nAccept: %s\r\nConnection: %s\r\n\r\n", m_pNodeCurr->dir.c_str(), strcmp(m_pNodeCurr->page.c_str(), "@")?m_pNodeCurr->page.c_str():"", m_pNodeCurr->host.c_str(), m_UserAgent, m_Accept, m_Connection);
	if(!DoneWithList(bWriteFile))
		return false;
	//AnalyzePage(NodeCurr);
	return true;
}
/**************************************************************
功能：设置 HTTP 协议头内容的一些固定值
***************************************************************/
int CNetSitAnalyse::GetLocalAgent(char* sUserAgent, 
                                  char* sAccept, 
                                  char* sAcceptLanguage, 
                                  char* sAcceptEncoding, 
                                  char* sAcceptCharset, 
                                  char* sKeepAlive, 
                                  char* sConnection, 
                                  char* sContentType)
{
	memcpy(sUserAgent, USERAGENT, strlen(USERAGENT));
	memcpy(sAccept, ACCEPT, strlen(ACCEPT));
	memcpy(sAcceptLanguage, ACCEPTLANGUAGE, strlen(ACCEPTLANGUAGE));
	memcpy(sAcceptEncoding, ACCEPTENCODING, strlen(ACCEPTENCODING));
	memcpy(sAcceptCharset, ACCEPTCHARSET, strlen(ACCEPTCHARSET));
	memcpy(sKeepAlive, KEEPALIVE, strlen(KEEPALIVE));
	memcpy(sConnection, CONNECTION, strlen(CONNECTION));
	memcpy(sContentType, CONTENTTYPE, strlen(CONTENTTYPE));
	return 0;
}
/**************************************************************
功能：显示年有节点信息
***************************************************************/
void CNetSitAnalyse::DisplayNode()
{
	fprintf(stdout,"\n");
	if(m_pNodeCurr)
	{
		if(!strcmp(m_pNodeCurr->dir.c_str(), "/"))	
			fprintf(stdout, "%s:%d%s%s => File size is:%d bytes\n", m_pNodeCurr->host.c_str(), m_pNodeCurr->port, m_pNodeCurr->dir.c_str(), strcmp(m_pNodeCurr->page.c_str(), "@")?m_pNodeCurr->page.c_str():"", m_pNodeCurr->nSize);
		else	
			fprintf(stdout, "%s:%d/%s/%s => File size is:%d bytes\n", m_pNodeCurr->host.c_str(), m_pNodeCurr->port, m_pNodeCurr->dir.c_str(), strcmp(m_pNodeCurr->page.c_str(), "@")?m_pNodeCurr->page.c_str():"", m_pNodeCurr->nSize);
	}
	//while(TempNode) 
	//{
	//	if(TempNode->child)	
	//		DisplayNode(TempNode->child);
	//	TempNode = TempNode->brother;
	//}
	if (DEBUG)
		fprintf(stdout,"Get count of errors is:%d",m_nError);

	return;
}
bool CNetSitAnalyse::DoneWithList(bool bWriteFile)
{
	fprintf(stdout, "Request.%d is:\n%s", ++m_nReqn, m_Request);

	if(!DoOnce(bWriteFile))
		return false;

	//if(flag) 
		//fprintf(stdout, "\nThe following is the response header:\n%s", m_Httpheader);
	return true;
}
/**************************************************************
功能：执行一次 HTTP 请求
***************************************************************/
bool CNetSitAnalyse::DoOnce(bool bWriteFile)
{
	/* send and receive */
	if (!ConnectWeb()) /* connect to the web server */
		return false;

	/* send a request */
	SendRequest();

	/* receive a response message from web server */
	if(!ReceiveResponse(bWriteFile))
		return false;

	close(m_nSockfd); /* because HTTP protocol do something one connection, so I can close it after receiving */
	return true;
}
/**************************************************************
功能：向网站发送 HTTP 请求
***************************************************************/
void CNetSitAnalyse::SendRequest(void)
{
	/* send my http-request to web server */
	  int ndsend = 0;
	  int ntotalsend = 0;
	  m_nBytes=strlen(m_Request);
	  while(ntotalsend < m_nBytes) 
	  {
	    ndsend = send(m_nSockfd, m_Request + ntotalsend, m_nBytes - ntotalsend,0);
	    if(ndsend==-1)  
	    {
	    	fprintf(stderr, "\tsend error!%s\n", strerror(errno));
	    	exit(0);
	    }
	    ntotalsend+=ndsend;
	    fprintf(stdout, "\nRequest.%d %d bytes send OK!\n", m_nReqn, ntotalsend);
	  }
	return;
}
/**************************************************************
功能：接收网站的 HTTP 返回
***************************************************************/
bool CNetSitAnalyse::ReceiveResponse(bool bWriteFile)
{
	fd_set writefds;
	struct timeval tival;
	int retry = 0;

	int i=0; 
	int j = 0;
	int nPos = 0;
	m_nSize = 0;
	int _ContentLengh = 0;
    while(m_nRet==0)
    {
        FD_ZERO(&writefds);
    	tival.tv_sec = 5;
    	tival.tv_usec = 0;
    	if(m_nSockfd > 0) 
    		FD_SET(m_nSockfd, &writefds);
    	else 
    	{
    		fprintf(stderr, "\n\tError, socket is negative!\n"); 
    		return false;
    	}

    	m_nRet = select(m_nSockfd + 1, &writefds, NULL, NULL, &tival);
    	if(m_nRet == 0 ) 
    	{
    		if ( retry==2 )
    			return false;
    	    if( retry++<2 ) 
    	    {
    	    	fprintf(stderr, "Error, socket is negative!\n"); 
    	    	continue;
    	    }
    	}
    	else
    		break;
    	
    }

	if(m_nRet <= 0) 
	{
	    fprintf(stderr, "\n\tError while receiving!\n"); 
		return false;
	}
	memset(m_BufferPool,0,sizeof(m_BufferPool));
	if(FD_ISSET(m_nSockfd, &writefds))    
	{
	    memset(m_Buffer, 0, sizeof(m_Buffer));
	    memset(m_Httpheader, 0, sizeof(m_Httpheader));
	   
	    /* receive data from web server */
	    
	    while((m_nBytes=recv(m_nSockfd,m_Buffer,1,0))==1)
	    {
	      if(i < 4)  
	      {
	    	  /* 获取 HTTP 消息头 */
	          if(m_Buffer[0] == '\r' || m_Buffer[0] == '\n')  
	        	  i++;
	          else 
	        	  i = 0;
	          memcpy(m_Httpheader + j, m_Buffer, 1); 
	          j++;
	      }
	      else
	    	  break;
	    }
		fprintf(stdout, "\nThe following is the response header:\n%s", m_Httpheader);
	    nPos = 0;
        _ContentLengh = GetNetDataLengh();
	    memset(m_Buffer,0,sizeof(m_Buffer));
	    memset(m_BufferPool,0,716801);
	    while((m_nBytes=recv(m_nSockfd,m_Buffer,sizeof(m_Buffer)-1,0))>0)
	    {

	    	 /*读取返回的内容*/
	    	 //fprintf(localfp, "%c", m_Buffer[0]); /* put content in the file */
	    	 
	    	if ((nPos + m_nBytes)>716801)
	    	{
	    		fprintf(stdout,"NetSitAnalyse is error,!\n");
	    		break;
	    	}
	    	 memcpy(m_BufferPool+nPos,m_Buffer,m_nBytes);
	    	 memset(m_Buffer,0,sizeof(m_Buffer));	
	    	 //sprintf(m_BufferPool + nPos,"%c",m_Buffer[0]);  
	    	 nPos += m_nBytes;
			 if (nPos == _ContentLengh-1)
				 break;
	    }
	_ContentLengh = nPos;
	    if (nPos!=0)
		{
			m_pNodeCurr->nSize = nPos + 1;
			m_nSize = nPos + 1;
			if (bWriteFile)
				WriteFile(m_BufferPool);
		}
	    }
	    m_nRet = 0;
	    return true;
}
bool CNetSitAnalyse::WriteFile(const char* sSrc)
{
	FILE * localfp = NULL;
	if((localfp = fopen(m_pNodeCurr->file.c_str(), "w")) == NULL) 
	{
		fprintf(stderr, "create file '%s' error\n", m_pNodeCurr->file.c_str()); 
		return false;
	}
	if (NULL == sSrc)
		return false;
	fprintf(localfp, "%s", m_Buffer);
	fclose(localfp);
	return true;
}
int CNetSitAnalyse::GetNetDataLengh()
{
	//std::string::size_type nPos = 0,nCurrPos = 0;
	int nCurrPos = 0;
	std::string _Content;
	char cNum[15];
	memset(cNum,0,sizeof(cNum));
	int i = 0;
	int _length = 0;

	_Content = m_Httpheader;
	if (_Content.empty())
		return 1;

	nCurrPos = _Content.find(CONTENTLENGTH);
	if (nCurrPos<=0)
		return 0;
	while (m_Httpheader[nCurrPos+sizeof(CONTENTLENGTH)+i]!=13)
		   //m_Httpheader[nCurrPos+sizeof(CONTENTLENGTH)+i]=='\n')
	{
		memcpy(cNum+i,m_Httpheader + nCurrPos + sizeof(CONTENTLENGTH) + i, 1);
		i++;
	}
    _length = atoi(cNum);
	return _length;
}
