//#include "stdafx.h"
#include "CSpiderPthread.h"
#include <assert.h>
#include "CRawData.h"

void* _spider_thread_proc(void* param);

CSpiderPthread::CSpiderPthread(DataQueue& _addrQueue, FilterObjectQueue&_rawFilterQueue,const int nInterval) :
	m_addrQueue(_addrQueue),
	m_rawFilterQueue(_rawFilterQueue)
{
	m_thread_id = 0;
	int ret = 0;
	m_nCrawlInterval = nInterval;
	ret = pthread_create(&m_thread_id, NULL, _spider_thread_proc, this);
	//m_hthread = CreateThread(NULL, 1024*1024, (LPTHREAD_START_ROUTINE)_spider_thread_proc, this, 0, &m_pthread_id);
	if (ret == -1)
	{
		fprintf(stderr, "_spider_thread_proc create thread failed.");
		assert(false);
	}
	fprintf(stdout, "CSpiderPthread create thread success!\n");
}

CSpiderPthread::~CSpiderPthread()
{
}

void* _spider_thread_proc(void* param)
{
	if (!param)
		return 0;
	
	CSpiderPthread* pObj = (CSpiderPthread*)param; 
	DataQueue::type_value pAddrObj = NULL; 
	unsigned long long _ulKey = 0;
	CRuleMgr rulemgr;
	std::string strTemp;
	char _cArray[1024];
	if (!rulemgr.loadRuleFromToFile("rule.xml"))
	{
		std::cout<<"Can't open XML file!"<<std::endl;
		return 0;
	}
	/*srand((unsigned)time(NULL));
	unsigned int _adr = 0;
	CData* pAddrObj = NULL;
			FILE* _pLocalf = NULL;
			_pLocalf = fopen("a","r+");
			if (_pLocalf == NULL)
				_pLocalf = fopen("a","w+");
			fseek( _pLocalf, 0, SEEK_END );
			int nSize = ftell( _pLocalf );
			fseek( _pLocalf, 0, SEEK_SET );
			
			char* szBuf = new char[nSize];
			memset( szBuf, 0, nSize );
			fread( szBuf, 1, nSize, _pLocalf );
			fclose( _pLocalf );
			
			char szIter;
			int nStart = 0;
			int nRow = 0;
			
			for( int i = 0; i < nSize; ++i )
			{
				szIter = *( szBuf + i );
				if( szIter == 10 )
				{
					for( int j = 0; j < i - nStart; j++ )
					{
						strTemp.append( szBuf + nStart + j, 1 );
					}
					nStart = i + 1;
					pObj->m_spider.GetHost(strTemp.c_str());
					pObj->m_spider.AddInitNode(true);   //true表示没有树杈
					pObj->m_spider.HandleInitNode(false); //是否写文件
					if (pObj->m_spider.GetDataLenth()>0)
					{
						memset(_cArray,0,sizeof(_cArray));
						memcpy(_cArray,strTemp.c_str(),strTemp.length());
						PAGELAYER pagelayer;
						INDEX unNetlay = 0;
						pagelayer.nCurrIndex = unNetlay;
						pagelayer.nCurrLayerIndex = unNetlay + 1;   //当前的层ID
						pagelayer.nFatherIndex = 1;
						pagelayer.sAddress = "";
						pagelayer.nRuleIndex = 0;        
						//
						pagelayer.sAddress = pObj->m_spider.GetAddrName();
						CFilter* pFilter = new CFilter(rulemgr); 
						//设置网页的层次 
						CRawData* addrdata = new CRawData();
						addrdata->put(strTemp.c_str());
						addrdata->setLayerInfo(pagelayer);
						pFilter->put(addrdata);
						pFilter->put(pObj->m_spider.GetData(),(long)pObj->m_spider.GetDataLenth(),pagelayer);
						pObj->m_rawFilterQueue.push(pFilter);
						}
						else
						{
							std::cout<<"Can't Get html!"<<std::endl;
							std::string _filename = "SpiderTrace.log";
						}
						//10~30
						int _nTime = (int)(30.0*rand_r(&_adr)/(RAND_MAX+10.0));
						::sleep(_nTime);
					  	//::usleep(pObj->m_nCrawlInterval);
						strTemp.clear();
					}
			  }*/
			while (true )
				{
					if (pObj->m_addrQueue.empty())
					{
						::sleep(1);
						continue;
					}
		
					pAddrObj = pObj->m_addrQueue.pop();
					if (pAddrObj)
					{			
					/// 在数据抓去下来之后需要new CFilter对象，并将原始数据传递到CFilter对象，然后提交
					/// 给存放需要分析数据的队列
					if (!pAddrObj->isIndexPage()&&pAddrObj->getType()==enAddrData)
					{
							_ulKey = url2id(pAddrObj->getData().c_str());
							if (neo_bdb_search(&_ulKey,_cArray) == 0)
									continue;
							else
							{
								///	这个obj是一个CAddrData 对象，调用他的getData()函数就直接可以得到地址字符窜
								/// getLayer()函数可以得到网页的层次关系
								///	...从网络上抓去原始数据
								INDEX unNetlay = pAddrObj->getLayer();
								pObj->m_spider.GetHost(pAddrObj->getData().c_str());
								pObj->m_spider.AddInitNode(true);   //true表示没有树杈
								pObj->m_spider.HandleInitNode(false); //是否写文件
								if (pObj->m_spider.GetDataLenth()>0)
								{
								memset(_cArray,0,sizeof(_cArray));
								memcpy(_cArray,pAddrObj->getData().c_str(),pAddrObj->getData().length());
								neo_bdb_insert(&_ulKey,_cArray);
								
								PAGELAYER pagelayer;
								pagelayer.nCurrIndex = unNetlay;
								pagelayer.nCurrLayerIndex = unNetlay + 1;   //当前的层ID
								pagelayer.nFatherIndex = 1;
								pagelayer.sAddress = "";
								pagelayer.nRuleIndex = 0;        //没有起到作用
								pagelayer.sAddress = pObj->m_spider.GetAddrName();
								CFilter* pFilter = new CFilter(rulemgr); 
								//设置网页的层次 
								pFilter->put(pAddrObj);
								pFilter->put(pObj->m_spider.GetData(),(long)pObj->m_spider.GetDataLenth(),pagelayer);
                pObj->m_rawFilterQueue.push(pFilter);
                strTemp.clear();
								}
								else
								{
										std::cout<<"Can't Get html!"<<std::endl;
										std::string _filename = "SpiderTrace.log";
										pObj->m_addrQueue.writeLog(_filename,pAddrObj->getData());
								}
								::usleep(pObj->m_nCrawlInterval);
					}
				}			
		   }
	   }
}
