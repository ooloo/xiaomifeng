#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "zlib.h"
#include "centerCache.h"
#include "hashtable.h"
#include "listpage.h"

#define MAX_HTML_LEN	(512*1024)
#define HASH_SLOT_NUM	5000011

typedef struct
{
	time_t m_tTimeIns;
	time_t m_tTimeUpd;
	int m_iOp;
	int m_iType;
	int m_iBBSId;
	int m_iLenUrl;
	int m_iLenTitle;
	int m_iLenHtml;
}html_entry;

typedef struct
{
	int linkNum;
	int pageType;
	char links[4096][MAX_URL_LEN];
}LINK_T;

typedef struct
{
	int fileLen;
	FILE *fp;
}HTMLFILE;

typedef struct
{
	int urlType;
	int bbsMysqlId;
}CHANNEL;

enum { ZONGHE = 0, LUNTAN = 1, NEWS = 2, OTHER = 3 } domainType;

int saveTime = 0;
unsigned tmpNo = 0;
int recvSock = -1;
int sendSock = -1;
int g_basePort = 0;
TDict *link_dict = NULL;
HTMLFILE saveFile = { 0, NULL };

pthread_mutex_t recv_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t send_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t html_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t hash_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t file_mutex=PTHREAD_MUTEX_INITIALIZER;

// use tmp...
void link_entities(char *link)
{
	char *p, *pp;

	pp = link;
	while((p=(char*)strcasestr(pp, "&amp;")))
	{
		p[1] = '\t';
		p[2] = '\t';
		p[3] = '\t';
		p[4] = '\t';
		pp = p + 5;
	}

	if(pp == link)
		return;

	p = pp = link;
	while(*p)
	{
		if(*p != '\t')
		{
			*pp = *p;
			pp++;
		}
		p++;
	}
	*pp = 0;

	return;
}

void save_html(char *filepath, URLNODE_T *urlNode, char *url, char *html)
{
	uLong destLen;
	int titleLen=0,stat;
	char cmd[256], filename[256], *dest;
	struct timeval tv;
	struct tm ttm;
	html_entry htmlEntry;

	dest = malloc(MAX_HTML_LEN);
	assert(dest);

	gettimeofday(&tv,NULL);
	localtime_r((time_t*)&tv,&ttm);
	snprintf(filename, 256, "%s/html.%d.%02d.%02d.%02d.%02d.%02d.%u", filepath,
			 ttm.tm_year+1900,ttm.tm_mon+1,ttm.tm_mday,ttm.tm_hour,ttm.tm_min,ttm.tm_sec, tmpNo++);

	destLen = MAX_HTML_LEN;
	assert(compress(dest, &destLen, html, urlNode->htmlLen) ==  Z_OK);

	if(saveFile.fp == NULL)
	{
		saveTime = time(NULL);
		saveFile.fp = fopen(filename, "wb");
		assert(saveFile.fp);
		saveFile.fileLen = 0;
	}
#ifdef BBS
	else if(saveFile.fileLen > 4*1024*1024 || time(NULL) - saveTime > 60)
#else
	else if(saveFile.fileLen > 4*1024*1024 || time(NULL) - saveTime > 1800)
#endif
	{
		fclose(saveFile.fp);

		snprintf(cmd, 256, "rename html one %s/html.*", filepath);
		system(cmd);

		saveTime = time(NULL);
		saveFile.fp = fopen(filename, "wb");
		assert(saveFile.fp);
		saveFile.fileLen = 0;
	}

	htmlEntry.m_tTimeIns	= urlNode->insertTime;
	htmlEntry.m_tTimeUpd	= urlNode->updateTime;
	htmlEntry.m_iOp			= urlNode->urlStat;
	htmlEntry.m_iType		= urlNode->urlType;
	htmlEntry.m_iBBSId		= urlNode->bbsMysqlId;
	htmlEntry.m_iLenUrl		= urlNode->urlLen;
	htmlEntry.m_iLenTitle	= 0;
	htmlEntry.m_iLenHtml	= destLen;

	assert(fwrite(&htmlEntry, sizeof(html_entry), 1, saveFile.fp) == 1);
	assert(fwrite(url, urlNode->urlLen, 1, saveFile.fp) == 1);
	assert(fwrite(dest, destLen, 1, saveFile.fp) == 1);

	saveFile.fileLen += sizeof(html_entry) + urlNode->urlLen + destLen;
	free(dest);
}

void link_insert(TDict *dict, char *keyStr, CHANNEL *chan)
{
	unsigned slotNo,key;
	TNodeInfo info;

	if(chan != NULL)
		info.dat = chan;
	else
		info.dat = NULL;

	assert(dict && keyStr);

	hashtable_hash(dict, &slotNo, &key, keyStr);

	assert(hashtable_insert(dict,slotNo,key,&info) != -1);

	return;
}

// return 1:found 0:notfound
int link_search(TDict *dict, char *keyStr, CHANNEL **chan)
{
	int ret;
	unsigned slotNo,key;
	TNodeInfo info;

	assert(dict && keyStr);

	hashtable_hash(dict, &slotNo, &key, keyStr);

	ret = hashtable_seek(dict,slotNo,key,&info);

	if(chan == NULL)
		return ret;

	if(ret == 1)
		*chan = info.dat;
	else
		*chan = NULL;

	return ret;
}

void link_init()
{
	FILE *fp;
	char *t,src[MAX_URL_LEN],path[MAX_URL_LEN];

	assert(link_dict=hashtable_init(HASH_SLOT_NUM));

	fp = fopen("produce.conf", "r");
	assert(fp);

	while(fgets(src, MAX_URL_LEN, fp))
	{
		sscanf(src, "%s", path);
		if(!strcasestr(path, "://"))
			continue;

		link_insert(link_dict, path, NULL);
		
		// path
		if(strchr(path + 7, '/') == NULL)
		{
			strcat(path, "/");
		}
		t = path;
		while(*t)
		{
			if(isupper(*t))
				*t += 32;
			t++;
		}
		t=strrchr(path, '/');
		assert(t);

		*(t+1) = 0;

		link_insert(link_dict, path, NULL);
	}

	fclose(fp);
}

int news_url_filter(char *url)
{
	char *p, *t;

	if(p=strchr(url, '?'))
	{
		p++;
		if(strchr(p, '?') || strlen(url) > 128)
		{
			return -1;
		}
	}
	else
	{
		if((p=strrchr(url + 7, '/')) == NULL)
			return -1;
		else if(*(p + 1) == 0)
			return -1;
		else if((t=strrchr(p, '.')) != NULL)
		{
			if(!strcasestr(t,"htm")
			   && !strcasestr(t,"jsp")
			   && !strcasestr(t,"asp")
			   && !strcasestr(t,"php"))
			{
				return -1;
			}
		}
	}

	return 0;
}

// return -1: need filter
int link_filter(char *url, int utype)
{
	int depth=0,dyn=0;;
	char *p,*pp,*t,*tt;
	char path[MAX_URL_LEN],star[MAX_URL_LEN];
	FILE *fp;

/*
	p = url + 7;
	while(*p)
	{
		if(*p == '/')
			depth++;
		if(*p == '?')
			dyn++;
		p++;
	}

	if(dyn > 1)
	{
		return -1;
	}
	else if(dyn == 0)
	{
		if((p=strrchr(url + 7, '/')) == NULL)
			return -1;
		else if((t=strrchr(p, '.')) != NULL)
		{
			if(!strcasestr(t,"htm")
			   && !strcasestr(t,"jsp")
			   && !strcasestr(t,"asp")
			   && !strcasestr(t,"php"))
			{
				return -1;
			}
		}
	}

	//if(utype == MANUAL)
	//	return 0;
	if(depth > 8 || strlen(url) > 128)
		return -1;
	else if(link_reg(url) == 0)
		return -1;
*/

	pp = url + 7;
	while(p=strchr(pp, '/'))
	{
		p++;
		assert(p - url < MAX_URL_LEN);
		strncpy(path, url, p - url);
		*(path + (p - url)) = 0;

		// Ä¿Â¼¹ýÂË
		if(link_search(link_dict, path, NULL) == 1)
		{
			return 0;
		}
		else if(pp == url + 7)
		{
	//		fp = fopen("domain.append","a");
	//		assert(fp);
	//		fprintf(fp, "%s\n", path);
	//		fclose(fp);
			tt = path + 7;
			while(t=strchr(tt,'.'))
			{
				snprintf(star, MAX_URL_LEN, "http://*%s", t);
				tt = t + 1;

				if(link_search(link_dict, star, NULL) == 1)
				{
					return 0;
				}
			}
		}
		pp = p;
	}

	return -1;
}

int node_recv(void *buf, int len, void *html, int maxLen)
{
	assert(buf && len && len > sizeof(URLNODE_T));

	pthread_mutex_lock(&recv_mutex);
	
	while(htcp_recv(recvSock, buf, len) == -1
		  || htcp_recv(recvSock, html, maxLen) == -1)
	{
		sleep(1);
		close(recvSock);

		while((recvSock = htcp_connect("127.0.0.1", g_basePort + 2)) == -1)
		{
			sleep(1);
			printf("htcp_connect error.\n");
		}
		printf("htcp_connect success.\n");
	}

	pthread_mutex_unlock(&recv_mutex);

	return 0;
}

int node_send(URLNODE_T *urlNode, char *url)
{
	int len;
	char *p,buf[2048];

	assert(urlNode && url);

	urlNode->urlLen = strlen(url);

	len = sizeof(URLNODE_T) + urlNode->urlLen;
	assert(len < 2048);

	memcpy(buf, urlNode, sizeof(URLNODE_T));
	memcpy(buf + sizeof(URLNODE_T), url, urlNode->urlLen);

	pthread_mutex_lock(&send_mutex);
	//printf("sending to center.............\n");
	while(htcp_send(sendSock, buf, len) == -1)
	{
		sleep(1);
		close(sendSock);

		while((sendSock = htcp_connect("127.0.0.1", g_basePort + 1)) == -1)
		{
			printf("htcp_connect error.\n");
			sleep(1);
		}
		printf("send htcp_connect success.\n");
	}
	pthread_mutex_unlock(&send_mutex);

	return 0;
}

void get_ganji_phone(URLNODE_T *urlNode, char *url, char *html)
{
	int ganjiLen=0;
	char *p=NULL,*t=NULL;
	char googleUrl[MAX_URL_LEN];
	char cmd[MAX_URL_LEN];
	FILE *fp;
	
	if((char*)strcasestr(url, "ganji.com") == NULL)
		return;

	if((p=(char*)strcasestr(html, "/info/tel_search")) == NULL)
		return;

	t=strchr(p, '\"');
	if(t == NULL)
		return;

	if(t-p >= MAX_URL_LEN)
		return;

	strncpy(googleUrl, p, t-p);
	googleUrl[t-p] = 0;
	
	unlink("/tmp/ganji.tmp");
	snprintf(cmd, MAX_URL_LEN, "wget -o /tmp/ganji.tmp -O /tmp/ganji.htm http://bj.ganji.com%s", googleUrl);
	system(cmd);

	printf("CMD: %s\n", cmd);

	fp = fopen("/tmp/ganji.tmp", "r");
	if(fp == NULL)
		return;

	ganjiLen = fread(html + urlNode->htmlLen, 1, MAX_HTML_LEN - urlNode->htmlLen - 8, fp);
	urlNode->htmlLen += ganjiLen;
	*(html + urlNode->htmlLen) = 0;

	fclose(fp);
		
	return;
}

int html_produce()
{
	int i, bbstime, listNum;
	char *p,buf[2048],cmd[MAX_URL_LEN],href[MAX_URL_LEN],resultURL[2048],*url,*html;
	FILE *fp;
	CHANNEL p_chan;
	URLNODE_T *urlNode;
	u_int8_t linkPickTime[4096];
	u_int8_t pageLinksType[4096];
	LINK_T *links = (LINK_T*)malloc(sizeof(LINK_T));
	assert(links);

	html = (char*)malloc(MAX_HTML_LEN);
	assert(html);

	urlNode = (URLNODE_T*)buf;
	url =  buf + sizeof(URLNODE_T);

	while(1)
	{
		//!!!!!!!!!!!!!!!!!!!!!!!
		usleep(50000);
		//!!!!!!!!!!!!!!!!!!!!!!!

		node_recv(buf, 2048, html, MAX_HTML_LEN);
		assert(urlNode->urlLen + sizeof(URLNODE_T) < 2048);
		assert(urlNode->htmlLen < MAX_HTML_LEN);

		*(url + urlNode->urlLen) = 0;
		printf("recv: %s urlLen: %d htmlLen: %d\n",url, urlNode->urlLen, urlNode->htmlLen);

		*(html + urlNode->htmlLen) = 0;

		// set zero
		bzero(links, sizeof(LINK_T));
		bzero(linkPickTime, 4096*sizeof(u_int8_t));
		bzero(pageLinksType, 4096*sizeof(u_int8_t));

		if(urlNode->htmlLen <= 0)
		{
			if(urlNode->urlStat == UPDATE && urlNode->urlType != MANUAL)
			{
				if(urlNode->statusCode >= 400 && urlNode->statusCode < 600)
					urlNode->urlStat = DELETE;

				pthread_mutex_lock(&file_mutex);
				save_html("./savefile", urlNode, url, html);
				pthread_mutex_unlock(&file_mutex);
			}
			urlNode->urlStat = UPDATE;
			node_send(urlNode, url);
			continue;
		}
		else if(urlNode->urlStat == UNCHANGE)
		{
			urlNode->urlStat = UPDATE;
			node_send(urlNode, url);
			continue;
		}

		switch(domainType)
		{
		case NEWS:
			{
				if(urlNode->urlType == MANUAL)
				{
					pthread_mutex_lock(&html_mutex);
					mylinkExtract_produce(url, html, urlNode->htmlLen, links);
					pthread_mutex_unlock(&html_mutex);
				}
				else
				{
					get_ganji_phone(urlNode, url, html);

					if(strstr(url, "bjfang.com") != NULL)
					{
						char *bjfang;
						bjfang = strstr(html, "gridview");
						if(bjfang != NULL)
						{
							*bjfang = 0;
							urlNode->htmlLen = strlen(html);
						}
					}

					pthread_mutex_lock(&file_mutex);
					save_html("./savefile", urlNode, url, html);
					pthread_mutex_unlock(&file_mutex);
				}
			}break;
		case LUNTAN:
			{
				if(urlNode->urlType == MANUAL)
				{
					pthread_mutex_lock(&html_mutex);
					
					// ------------ button page -------------
#ifndef BBS
				/*	mylinkExtract_produce(url, html, urlNode->htmlLen, links);

					listNum = 0;
					char *u1,*u2;
					u1 = strrchr(url + 7, '/');

					for(i=0; u1!=NULL && i<links->linkNum && listNum <= 5; i++)
					{
						link_entities(links->links[i]);
						if((u2 = strrchr(links->links[i] + 7, '/')) == NULL)
							continue;
						if(u2[1] == 0
						   || strlen(links->links[i]) < strlen(url)
						   || strncasecmp(links->links[i], url, u1 - url) != 0)
							continue;

						if(((char*)strstr(links->links[i], url) != NULL && strcasestr(u2, "page="))
						   || get_web_page_similarity((const char*)u1, strlen(u1),
													  (const char*)u2, strlen(u2)) > 90)
						{
							if(i > listNum)
								strcpy(links->links[listNum], links->links[i]);

							pageLinksType[listNum] = MANUAL;
							listNum++;
						}
					}
					links->linkNum = listNum; */
#endif
					// ------------ button page -------------

					snprintf(cmd, MAX_URL_LEN, "./bbs/%d/", urlNode->bbsMysqlId);
					mkdir(cmd, 0777);

					strcat(cmd, "tmp.htm");
					fp = fopen(cmd, "w");
					assert(fp);
					fwrite(html, 1, urlNode->htmlLen, fp);
					fclose(fp);

					snprintf(cmd, MAX_URL_LEN, "./bbs/%d/tmp_s.link", urlNode->bbsMysqlId);
					unlink(cmd);

					snprintf(cmd, MAX_URL_LEN, "./1205 -tf ./bbs/%d/tmp.htm", urlNode->bbsMysqlId);
					system(cmd);

					snprintf(cmd, MAX_URL_LEN, "./bbs/%d/tmp_s.link", urlNode->bbsMysqlId);
					fp = fopen(cmd, "r");
					if(fp == NULL)
					{
						printf("failed to extract link (%s)\n", url);
					}
					else
					{
						while(fgets(cmd, MAX_URL_LEN, fp) != NULL)
						{
							sscanf(cmd, "%d %s", &bbstime, href);
							if(mylink_format(url, href, resultURL) == 0)
							{
								if(strlen(resultURL) >= MAX_URL_LEN)
									continue;
#ifdef BBS
								if(time(NULL) - bbstime > 86400 && bbstime > 0)
									continue;
#endif
								if(bbstime <= 0)
								{
									linkPickTime[links->linkNum] = BBSNOTIME;
								}
								else
								{
									linkPickTime[links->linkNum] = BBSPUBTIME;
								}

								strcpy(links->links[links->linkNum], resultURL);

								link_entities(links->links[links->linkNum]);
								links->linkNum++;
							}
						}
						fclose(fp);
					}
					pthread_mutex_unlock(&html_mutex);
				}
				else
				{
#ifdef BBS
					bbstime = urlNode->updateTime;
					if(urlNode->linkPickInfo == BBSNOTIME)
					{
						bbstime = get_pub_time(html, urlNode->updateTime);
					}
					if(time(NULL) - bbstime < 86400)
					{
						pthread_mutex_lock(&file_mutex);
						save_html("./savefile", urlNode, url, html);
						pthread_mutex_unlock(&file_mutex);
					}
#else
					pthread_mutex_lock(&file_mutex);
					save_html("./savefile", urlNode, url, html);
					pthread_mutex_unlock(&file_mutex);
#endif
				}
			}break;
		case ZONGHE:
			{
				pthread_mutex_lock(&html_mutex);
				mylinkExtract_produce(url, html, urlNode->htmlLen, links);
				pthread_mutex_unlock(&html_mutex);

				// check index page
				if(links->pageType == 0 && links->linkNum >= 256)
				{
					if(urlNode->urlType == NORMAL)
					{
						urlNode->urlType = INDEX;
						urlNode->urlRank = 0;	// it is a flag, change rank at center
					}
				}
				else
				{
					if(urlNode->urlType == INDEX)
					{
						urlNode->urlType = NORMAL;
						urlNode->urlRank = 0;	// it is a flag, change rank at center
					}
				}

				if(links->pageType == 0)
					printf("INDEX PAGE %d >>> (%s)\n", links->linkNum, url);

				pthread_mutex_lock(&file_mutex);
				save_html("./savefile", urlNode, url, html);
				pthread_mutex_unlock(&file_mutex);
			}break;
		default:
			{
				pthread_mutex_lock(&file_mutex);
				save_html("./savefile", urlNode, url, html);
				pthread_mutex_unlock(&file_mutex);
			}
		}

		// update this url
		urlNode->linkNum = links->linkNum;
		urlNode->urlStat = UPDATE;
		p_chan.urlType = urlNode->urlType;
		p_chan.bbsMysqlId = urlNode->bbsMysqlId;
		node_send(urlNode, url);
		printf("extract %d links from (%s)\n", links->linkNum, url);
		
		// new links
		for(i=0; i<links->linkNum; i++)
		{
			pthread_mutex_lock(&hash_mutex);
			if(domainType == ZONGHE)
			{
				if(link_filter(links->links[i], p_chan.urlType) == -1)
				{
					pthread_mutex_unlock(&hash_mutex);
					continue;
				}
			}
			else if(domainType == LUNTAN)
			{
				char *p;
				p = (char*)strcasestr(links->links[i], "&cachetime=");
				if(p != NULL)
					*p = 0;
				p = (char*)strcasestr(links->links[i], "?time=");
				if(p != NULL)
					*p = 0;
			}
			else if(domainType == NEWS)
			{
				// soufun.com
				//if(strstr(links->links[i], "soufun.com") != NULL
				//		&& (strstr(links->links[i], "rent/searchmore") != NULL 
				//			|| strstr(links->links[i], "newsecond/searchmore")) != NULL)
				//{
				//	pageLinksType[i] = MANUAL;
				//}
				if(strstr(links->links[i], "bjfang.com") != NULL 
						&& (strchr(links->links[i], '?') != NULL || strchr(links->links[i], '&') != NULL))
				{
					pthread_mutex_unlock(&hash_mutex);
					continue;
				}
				else if(link_filter(links->links[i], p_chan.urlType) == -1)
				{
					pthread_mutex_unlock(&hash_mutex);
					continue;
				}

				// -- 2008.10.20 --
				//if(insite_check(url, links->links[i]) <= 0
				//   || news_url_filter(links->links[i]) == -1)
				//{
				//	pthread_mutex_unlock(&hash_mutex);
				//	continue;
				//}
				// -- 2008.10.20 --
			}

			if(link_search(link_dict, links->links[i], NULL) == 1)
			{
				pthread_mutex_unlock(&hash_mutex);
				continue;
			}
			else
			{
				if(link_dict->nodeNum < 100000)
				{
					link_insert(link_dict, links->links[i], NULL);
				}
				else
				{
					hashtable_free(link_dict);
					link_init();
				}
			}
			pthread_mutex_unlock(&hash_mutex);

			bzero(urlNode, sizeof(URLNODE_T));
			urlNode->urlLen			= strlen(links->links[i]);
			urlNode->urlType		= pageLinksType[i];
			urlNode->urlStat		= INSERT;
			urlNode->bbsMysqlId		= (unsigned short)p_chan.bbsMysqlId;
			urlNode->linkPickInfo	= linkPickTime[i];
			node_send(urlNode, links->links[i]);
			printf("send type=%d (%s) to center\n", pageLinksType[i], links->links[i]);
		}
	}

	free(html);
	return 0;
}

int main(int argc, char *argv[])
{
	int i,ret;
	pthread_t id;

	if(argc != 3)
	{
		printf("%s baseport luntan|zonghe|news|other\n", argv[0]);
		exit(1);
	}

	mylinkExtract_init();

	if(strcasecmp(argv[2], "luntan") == 0)
		domainType = LUNTAN;
	else if(strcasecmp(argv[2], "zonghe") == 0)
		domainType = ZONGHE;
	else if(strcasecmp(argv[2], "news") == 0)
		domainType = NEWS;
	else if(strcasecmp(argv[2], "other") == 0)
		domainType = OTHER;
	else
	{
		printf("%s baseport luntan|zonghe|news|other\n", argv[0]);
		exit(1);
	}

	link_init();
	printf("neo init success\n");

	//ret=link_filter("http://wow.tgbus.com/class/shaman", 0);
	//printf("filter: %d\n", ret);
	//char resultURL[1024];
	//mylink_format("http://edu.966151.cn/browse/2/?type=1", "../../question.php?qid=606080&", resultURL);
	//printf("%s\n", resultURL);
	//exit(1);

	g_basePort = atoi(argv[1]);
	assert(g_basePort > 1024 && g_basePort < 65535);

	for(i=0; i<8; i++)
	{
		ret = pthread_create(&id, NULL, (void *)html_produce, NULL);
		if(ret != 0)
		{
			printf("create thr error.\n");
			exit(1);
		}
	}

	pthread_join(id, NULL);

	return 0;
}
