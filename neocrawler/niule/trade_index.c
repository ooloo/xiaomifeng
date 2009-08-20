#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <pthread.h>
#include <openssl/md5.h>
#include "zlib.h"
#include "nlpub.h"
#include "newdict.h"
#include "neopub.h"
#include "tradebdb.h"
#include "Hootcp.h"
#include "rbdict.h"
#include "tradedecode.h"

#define MIN_QUERY_LEN 2

typedef struct _node
{
	unsigned long long uid;
	int trPubTime;
}trade_node_t;

typedef struct _key
{
	unsigned key;
}trade_key_t;

typedef struct _dat
{
	trade_node_t *node;
	struct _dat *next;
}trade_dat_t;

typedef struct _index
{
	TRbDict *word_index;
	trade_key_t *tradeKey;
	trade_dat_t *tradeDat;
	trade_node_t *tradeNode;
	int keyNum;
	int datNum;
	int nodeNum;
}trade_index_t;

int printNum=0, pageNo=0, leapNum=0, showNum=10;
int userSock=-1;
char username[128];

dict_t g_pstDict;
trade_index_t g_tradeIndex={NULL};
pthread_mutex_t index_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t word_mutex=PTHREAD_MUTEX_INITIALIZER;

int poscomp(const void *pos1, const void *pos2)
{
	return (*(int*)pos2 - *(int*)pos1);
}

int key_cmp(const void *key1, const void *key2)
{
	trade_key_t *k1,*k2;
	k1 = (trade_key_t*)key1;
	k2 = (trade_key_t*)key2;

	if(k1->key < k2->key)
		return 1;
	else if(k1->key > k2->key)
		return -1;
	else
		return 0;
}

static int compmi(const void *m1, const void *m2)
{
	trade_node_t *mi1 = (trade_node_t*)m1;
	trade_node_t *mi2 = (trade_node_t*)m2;

	if(mi1->trPubTime < mi2->trPubTime)
		return 1;
	else if(mi1->trPubTime == mi2->trPubTime)
	{
		if(mi1->uid < mi2->uid)
			return 1;
		else if(mi1->uid == mi2->uid)
			return 0;
		else
			return -1;
	}
	else
		return -1;
}

int trdate_index_init()
{
	g_tradeIndex.word_index = rb_dict_new((void*)key_cmp, NULL, NULL);
	assert(g_tradeIndex.word_index);

	g_tradeIndex.tradeKey = (trade_key_t*)calloc(5000000, sizeof(trade_key_t));
	assert(g_tradeIndex.tradeKey);

	g_tradeIndex.tradeDat = (trade_dat_t*)calloc(20000000, sizeof(trade_dat_t));
	assert(g_tradeIndex.tradeDat);

	g_tradeIndex.tradeNode = (trade_node_t*)calloc(5000000, sizeof(trade_node_t));
	assert(g_tradeIndex.tradeNode);

	g_tradeIndex.keyNum = 0;
	g_tradeIndex.datNum = 0;
	g_tradeIndex.nodeNum = 0;
	
	return 0;
}

int trade_insert_one(TRbDict *dict, trade_key_t *key, trade_dat_t *dat, trade_node_t *node)
{
	trade_dat_t *tmp;

	if(rb_dict_search(dict, key, (void**)&tmp) == 0)
	{
		dat->node = tmp->node;
		tmp->node = node;
		dat->next = tmp->next;
		tmp->next = dat;

		g_tradeIndex.datNum++;
	}
	else
	{
		dat->node = node;
		rb_dict_insert(dict, key, dat, 0); 

		g_tradeIndex.keyNum++;
		g_tradeIndex.datNum++;
	}
}

int trade_index_insert(INDEXDATA *node, word_t *pword, int iword)
{
	int i;

	pthread_mutex_lock(&index_mutex);
	
	g_tradeIndex.tradeNode[g_tradeIndex.nodeNum].uid = url2id(node->url);
	g_tradeIndex.tradeNode[g_tradeIndex.nodeNum].trPubTime = node->insertTime;

	for(i=0; i<iword; i++)
	{
		if(strlen(pword[i].m_szWord) >= MIN_QUERY_LEN)
		{
			//printf("word = %s \n", pword[i].m_szWord);
			g_tradeIndex.tradeKey[g_tradeIndex.keyNum].key = str2key(pword[i].m_szWord);
			trade_insert_one(g_tradeIndex.word_index, &g_tradeIndex.tradeKey[g_tradeIndex.keyNum],
							 &g_tradeIndex.tradeDat[g_tradeIndex.datNum], &g_tradeIndex.tradeNode[g_tradeIndex.nodeNum]);
		}
	}

	g_tradeIndex.nodeNum++;
	
	pthread_mutex_unlock(&index_mutex);

	return 0;
}

void summary_put(INDEXDATA *node, char *summary)
{
	uLong bufLen=0, srcLen=0;
	char *buf=NULL,*src=NULL;
	unsigned long long key;

	bufLen =  MAX_HTML_LEN;
	buf = (char*)malloc(bufLen);
	assert(buf);

	src = (char*)malloc(bufLen);
	assert(src);

	memcpy(src, node, sizeof(INDEXDATA));
	srcLen += sizeof(INDEXDATA);
	memcpy(src + srcLen, summary, node->summaryLen);
	srcLen += node->summaryLen;
	
	assert(compress(buf, &bufLen, (char*)src, srcLen) ==  Z_OK);

	key = url2id(node->url);
	assert(key);

	neo_bdb_bin_insert(&key, buf, bufLen);

	free(buf);
	free(src);
}

void summary_get(unsigned long long *key, INDEXDATA *node, char *summary)
{
	int bufLen=0;
	char *buf=NULL,*src=NULL;

	bufLen =  MAX_HTML_LEN;
	buf = (char*)malloc(bufLen);
	assert(buf);

	src = (char*)malloc(bufLen);
	assert(src);

	bzero(node, sizeof(INDEXDATA));
	*summary = 0;

	if(-1 != neo_bdb_bin_search(key, buf, &bufLen))
	{
		ulong srcLen = MAX_HTML_LEN;
		assert(uncompress((Bytef*)src, &srcLen, (Bytef*)buf, bufLen) == Z_OK);

		memcpy(node, src, sizeof(INDEXDATA));
		assert(srcLen == sizeof(INDEXDATA) + node->summaryLen);
		memcpy(summary, src + sizeof(INDEXDATA), node->summaryLen);
		*(summary + node->summaryLen) = 0;
	}

	free(buf);
	free(src);
}

int summary_calculate(char *summary, char *res, word_t *pword, int iword)
{
	int i,j,perLen,summaryLen,resLen=0;
	char *pos[10];
	char *start, *end;

	summaryLen = strlen(summary);

	for(i=0; i<iword&&i<10; i++)
	{
		pos[i] = strstr(summary, pword[i].m_szWord);
	}

	qsort(pos, i, sizeof(char*), poscomp);

	// 220 char, put into res (1024)
	for(j=0; j<i; j++)
	{
		if(pos[j] == NULL)
			continue;
		else
			break;
	}
	
	perLen = 196 / (i-j+1);

	end = summary;
	for(; j<i; j++)
	{
		start = end > (pos[j] - perLen) ? end : (pos[j] - perLen);
		end = (start + perLen * 2) < (summary + summaryLen) ? (start + perLen * 2) : (summary + summaryLen);

		//while(*start != ' ' && start < pos[j])
		while((*(unsigned char*)start >= 'z') && start < pos[j])
			start++;

		while((*(unsigned char*)end >= 'z') && *end != 0)
			end++;

		memcpy(res + resLen, start, end - start);
		resLen +=  end - start;

		memcpy(res + resLen, " ...", 4);
		resLen +=  4;
	}

	*(res + resLen) = 0;

	if(resLen > 300)
	{
		*(res + 220) = 0;
		strcat(res, " ...");
	}
	else if(resLen == 0)
	{
		start = summary + summaryLen/8;
		end = summary + summaryLen;
		
		while((*(unsigned char*)start >= 'z') && start < end)
			start++;

		end = start + 196;
		while((*(unsigned char*)end >= 'z') && *end != 0)
			end++;

		resLen =  end - start;
		if(resLen > 300)
		{
			snprintf(res, 220, "... %s", start);
			strcat(res, " ...");
		}
		else
		{
			memcpy(res, start, end - start);
			*(res + resLen) = 0;
			strcat(res, "...");
		}
	}

	return 0;
}

int trade_recv_thr()
{
	int len, bufLen, iword;
	int recvSock=-1, recvListenSock=-1;
	char *recvBuf=NULL;
	word_t *pword;
	INDEXDATA *node;

	bufLen = MAX_HTML_LEN;
	recvBuf = (char*)malloc(bufLen);
	assert(recvBuf);

	node = (INDEXDATA*)recvBuf;
	pword = (word_t*)(recvBuf + sizeof(INDEXDATA));

	recvListenSock = htcp_listen(9527, 5);
	assert(recvListenSock);
	printf("recv htcp_listen success [%d].\n", 9527);

	while( 1 )
	{
		recvSock = htcp_accept(recvListenSock, NULL, NULL);
		if(recvSock == -1)
		{
			printf("accept error.\n");
			sleep(1);
			continue;
		}
		printf("send htcp_accept success.\n");

		while(htcp_recv(recvSock, recvBuf, bufLen) != -1)
		{
			printf("index (%s).\n", node->url);
			summary_put(node, recvBuf + sizeof(INDEXDATA) + node->wordNum * sizeof(word_t));

			//printf("---------------------------------------\n");
			//printf("wordNum  =============  %d\n", node.wordNum);

			trade_index_insert(node, pword, node->wordNum);
		}

		close(recvSock);
	}

	free(recvBuf);
	return 0;
}

int visit(const void *key, void *dat, void *args)
{
	char res[MAX_ITEM_LEN];
	INDEXDATA node;
	trade_dat_t *tmp;
	//printf("%u %llu\n", *(unsigned*)key, ((trade_dat_t*)dat)->node->uid);

	tmp = (trade_dat_t*)dat;

	while(tmp != NULL)
	{
		if(leapNum++ < (pageNo-1) * showNum)
		{
			tmp = tmp->next;
			continue;
		}

		if(-1 != neo_bdb_search_t(tmp->node, res))
		{
			if(printNum++ > showNum)
			{
				return -1;
			}
			else
			{
			}
		}
		tmp = tmp->next;
	}

	return 0;
}

int check_user_urllist(char *url)
{
	char buf[MAX_ITEM_LEN];
	if(userSock == -1)
	{
		if((userSock = htcp_connect("127.0.0.1", 9827)) == -1)
		{
			printf("userinfo server error!!!\n");
			return -1;
		}
	}

	snprintf(buf, MAX_ITEM_LEN, "%s %s", url, username);

	if(htcp_send(userSock, buf, strlen(buf) + 1) == -1)
	{
		printf("send failed. (%s)\n", buf);
		close(userSock);
		userSock = -1;
		return -1;
	}

	if(htcp_recv(userSock, buf, MAX_ITEM_LEN) == -1)
	{
		printf("send failed. (%s)\n", buf);
		close(userSock);
		userSock = -1;
		return -1;
	}

	if(*buf == 0)
		return 0;
	else
		return 1;
}

int merge(int searchSock, word_t *pword, int iword)
{
	int i=0,k=0,resultNum=0,maxmergenum=0;
	int sendNum=0,mergeNum=0,maxNum=0;
	char res[MAX_ITEM_LEN];
	char *summary=NULL;
	trade_node_t *mergeNode=NULL;

	maxmergenum = 1024;
	mergeNode = (trade_node_t*)malloc(maxmergenum * sizeof(trade_node_t));
	assert(mergeNode);

	summary = (char*)malloc(MAX_HTML_LEN);
	assert(summary);

	trade_key_t key;
	trade_dat_t *tmp;
	INDEXDATA node;

	for(i=0; i<iword; i++)
	{
		key.key = str2key(pword[i].m_szWord);
		if(rb_dict_search(g_tradeIndex.word_index, &key, (void**)&tmp) == 0)
		{
			while(tmp != NULL)
			{
				if(mergeNum >= maxmergenum - 2)
				{
					if(maxmergenum * 2 >= 65536*4 + 2 || iword <= 1)
					{
						break;
					}
					maxmergenum *= 2;
					mergeNode = (trade_node_t*)realloc(mergeNode, maxmergenum * sizeof(trade_node_t));
					assert(mergeNode);
				}
				memcpy(&mergeNode[mergeNum++], tmp->node, sizeof(trade_node_t));
				tmp = tmp->next;
			}
		}
		printf("mergeNum = %d word = %s\n", mergeNum, pword[i].m_szWord);
	}

	// mergeNum + 1 < maxmergenum
	bzero(&mergeNode[mergeNum], sizeof(trade_node_t));
	qsort(mergeNode, mergeNum, sizeof(trade_node_t), compmi);
	for(i=0,k=0; i<mergeNum; i++)
	{
		if(compmi(&mergeNode[i], &mergeNode[i+1]) == 0)
		{
			k++;
			printf("max's id = %llu\n", mergeNode[i].uid);
			printf("max's id = %llu\n", mergeNode[i+1].uid);
			if(maxNum < k)
			{
				maxNum = k;
				resultNum = 1;
			}
		}
		else
		{
			k=0;
		}
		mergeNode[i].trPubTime = k;
		if(k == maxNum)
			resultNum++;
	}
	printf("maxNum = %d iword = %d pageNum = %d\n", maxNum, iword, pageNo);

	for(i=0,k=0; i<mergeNum; i++)
	{
		if(mergeNode[i].trPubTime == maxNum)
		{
			if(++k < (pageNo-1) * showNum)
				continue;

			summary_get(&mergeNode[i].uid, &node, summary);
			summary_calculate(summary, res, pword, iword);

			if(*username != 0)
			{
				if(check_user_urllist(node.url) == 0)
					continue;
			}

			if(sendNum++ > showNum)
			{
				break;
			}
			else
			{
				char sendBuff[4096];
				snprintf(sendBuff, 4096, "<item>\n"
						"<page><![CDATA[%d]]></page>\n"
						"<shownum><![CDATA[%d]]></shownum>\n"
						"<totalnum><![CDATA[%d]]></totalnum>\n"
						"<url><![CDATA[%s]]></url>\n"
						"<title><![CDATA[%s]]></title>\n"
						"<summary><![CDATA[%s ]]></summary>\n"
						"</item>\n",
						pageNo, showNum, resultNum, node.url, node.title, res);
				
				printf("%s", sendBuff);
				htcp_send(searchSock, sendBuff, strlen(sendBuff) + 1);
			}
		}
	}

	free(mergeNode);
	free(summary);

	return 0;
}

void trade_search_thr(int searchSock)
{
	int i, iword=0;
	char *p, *t;
	char *page=NULL,*user=NULL, query[1024];
	word_t pword[MAX_WORD_NUM];

	pthread_detach(pthread_self());

	if(htcp_recv(searchSock, query, 1024) == -1)
	{
		close(searchSock);
		return;
	}
	printf("query = %s\n", query);

	page=strstr(query, "&page=");
	user=strstr(query, "&usrid=");

	if((p=strstr(query, "search=")) != NULL)
	{
		p += strlen("search=");
		t = strchr(p, '&');
		if(t != NULL)
			*t = 0;

		if(strlen(p) > 1)
		{
			decodeUrl(p);

			printf("decode = %s\n", p);

			pthread_mutex_lock(&word_mutex);
			g_pstDict.m_iCurrCount = g_pstDict.m_iWordCount;
			g_pstDict.m_iSentCount = g_pstDict.m_iSentTotal;

			int iRetCode;
			iRetCode = cwlib_seg_word(&g_pstDict, p, strlen(p),
									  &(g_pstDict.m_pstWord), &(g_pstDict.m_iCurrCount),
									  &(g_pstDict.m_pstSent), &(g_pstDict.m_iSentCount));

			if(0 != iRetCode)
			{
				iword = 0;
			}
			else
			{
				// ignore inner & little words
				for(i=0; i<g_pstDict.m_iCurrCount && iword < MAX_WORD_NUM;)
				{
					if(strlen(g_pstDict.m_pstWord[i].m_szWord) < MIN_QUERY_LEN)
					{
						i++;
						continue;
					}

					memcpy(&pword[iword++], &g_pstDict.m_pstWord[i++], sizeof(word_t));
					i += pword->m_tProp.m_tInner;
				}
			}
			pthread_mutex_unlock(&word_mutex);
		}
	}

	pthread_mutex_lock(&index_mutex);
	printNum = 0;	leapNum = 0;

	if(page != NULL)
		pageNo = atoi(page + strlen("&page="));
	else
		pageNo = 1;
	pageNo = pageNo > 1 ? pageNo : 1;

	if(user != NULL)
	{
		user += strlen("&usrid=");
		t = strchr(user, '&');
		if(t != NULL)
			*t = 0;
		snprintf(username, 128, "%s", user);
	}
	else
	{
		*username = 0;
	}

	merge(searchSock, pword, iword);

	pthread_mutex_unlock(&index_mutex);

	close(searchSock);

	printf("this client exit.\n");
}

void trade_search_accept()
{
	int searchSock, searchListenSock;
	pthread_t id;

	searchListenSock = htcp_listen(9627, 5);
	assert(searchListenSock);
	printf("search htcp_listen success [%d].\n", 9627);

	while(1)
	{
		if((searchSock = htcp_accept(searchListenSock, NULL, NULL)) == -1)
		{
			printf("accept error.\n");
			sleep(1);
		}
		printf("search htcp_accept success.\n");

		if(pthread_create(&id, NULL, (void *)trade_search_thr, (void *)searchSock) != 0)
		{
			printf("create search_thr thr error.\n");
			exit(1);
		}
	}

	return;
}

int main(int argc, char** argv)
{
	pthread_t id;
	char *szPath = "../dict/";

	trdate_index_init();

	g_pstDict.m_iWordCount = MAX_WORD_NUM;
	g_pstDict.m_iSentTotal = MAX_WORD_NUM;
	if (0 != dict_load(szPath, &g_pstDict))
	{
		printf("dict_load(%s) failed\n", szPath);
		exit(1);
	}
	else
	{
		printf("dict_load OK!\n");
	}

	neo_bdbenv_init("/home2/indexbdb/");
	neo_bdb_init("/home2/indexbdb/", DB_CREATE);
	printf("bdb open success.\n");

	if(pthread_create(&id, NULL, (void *)trade_recv_thr, NULL) != 0)
	{
		printf("create trade_recv_thr error.\n");
		exit(1);
	}

	if(pthread_create(&id, NULL, (void *)trade_search_accept, NULL) != 0)
	{
		printf("create trade_recv_thr error.\n");
		exit(1);
	}

	pthread_join(id, NULL);

	return 0;
}
