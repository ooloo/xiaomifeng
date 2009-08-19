#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <strings.h>
#include <time.h>
#include "zlib.h"
#include "tradebdb.h"
#include "neopub.h"
#include "Hootcp.h"
#include "nlpub.h"
#include "newdict.h"


int count=0;
FILE *idxfp=NULL;

int sendSock=-1;
char *sendBuf=NULL;

int pword_compare(const void *pword1, const void *pword2)
{
	return strcoll(((word_t*)pword1)->m_szWord, ((word_t *)pword2)->m_szWord);
}

int get_one_html(FILE *fp, html_entry *htmlNode, char *url, char *title, char *html, char *comphtml)
{
	uLong htmlLen;

	if(fread(htmlNode, sizeof(html_entry), 1, fp) != 1)
	{
		return -1;
	}

	*url = 0; *title = 0; *html = 0; *comphtml = 0;

	assert(fread(url, htmlNode->m_iLenUrl , 1, fp) == 1);
	url[htmlNode->m_iLenUrl] = 0;

	printf("(%d) %s\n", ++count, url);

	if(htmlNode->m_iLenTitle > 0)
	{
		assert(fread(title, htmlNode->m_iLenTitle , 1, fp) == 1);
		title[htmlNode->m_iLenTitle] = 0;
	}

	if(htmlNode->m_iLenHtml > 0)
	{
		assert(fread(comphtml, htmlNode->m_iLenHtml , 1, fp) == 1);

		htmlLen = MAX_HTML_LEN;
		assert(uncompress((Bytef*)html, &htmlLen, (Bytef*)comphtml, htmlNode->m_iLenHtml) == Z_OK);

		html[htmlLen] = 0;
	}

	return 0;
}

int tradenode_send(INDEXDATA *node, word_t *pword, char *summary)
{
	int len=0, iword=0;

	iword = node->wordNum;

	if(sendBuf == NULL)
	{
		sendBuf = (char*)malloc(MAX_HTML_LEN);
		assert(sendBuf);
	}

	assert(sizeof(INDEXDATA) + iword * sizeof(word_t) + node->summaryLen < MAX_HTML_LEN);

	memcpy(sendBuf, node, sizeof(INDEXDATA));
	len += sizeof(INDEXDATA);

	memcpy(sendBuf + len, pword, iword * sizeof(word_t));
	len += iword * sizeof(word_t);

	memcpy(sendBuf + len, summary, node->summaryLen + 1);
	len += node->summaryLen + 1;

	while(htcp_send(sendSock, sendBuf, len) == -1)
	{
		sleep(1);
		close(sendSock);

		while((sendSock = htcp_connect("127.0.0.1", 9527)) == -1)
		{
			printf("htcp_connect error.\n");
			sleep(1);
		}
		printf("send htcp_connect success.\n");
	}

	return 0;
}

int trim(char *src, char *dest)
{
	int i = 0;
	char *p = src;

	while(*p && isspace(*p) != 0)
		p++;
	
	while(*p)
	{
		if(isspace(*p)==0 || isspace(dest[i])==0)
		{
			if(isspace(*p)==0)
				dest[i++] = ' ';
			else
				dest[i++] = *p;
		}
		p++;
	}

	dest[i] = 0;
}

int trim_sent(char *src, char *dest, int destLen)
{
	int i = 0;
	char *p = src;

	while(*p && isspace(*p) != 0)
		p++;
	
	while(*p && i < destLen - 1)
	{
		if(i == 0)
		{
			dest[i++] = *p;
		}
		else if(isspace(*p)==0 || isspace(dest[i-1])==0)
		{
			if(isspace(*p)!=0)
				dest[i++] = ' ';
			else
				dest[i++] = *p;
		}
		p++;
	}

	dest[i] = 0;
}

void get_summary(char *context, char *summary)
{
	unsigned short H;
	char *s = context;

	while(*s != 0)
	{
		memcpy(&H, s, 2);
		
		if(((H&0x00ff)==0x00a3&&(H&0xff00)>=0xa100)||((H&0x00ff)==0x00a1&&(H&0xff00)>=0xa100))
		{
			if(H == 0xa1a1)
			{
				memcpy(summary, " ", 1);
				summary++;
				s+=2;
			}
			else if(H == 0xa1a3 || H == 0xa3a1 || H == 0xbfa3 || H == 0xaca3)
			{
				memcpy(summary, s, 2);
				summary+=2;
				memcpy(summary, " ", 1);
				summary++;
				s+=2;
			}
			else
			{
				memcpy(summary, s, 2);
				summary+=2;
				s+=2;
			}
		}
		else if((H&0x00ff)>=0x00b0&&(H&0x00ff)<=0x00f7&&(H&0xff00)>=0xa100)
		{
			memcpy(summary, s, 2);
			summary+=2;
			s+=2;
		}
		//else if((H&0x00ff)<0x00a1)
		//else if(*s == ' ')
		else if(*s >=' ' && *s <= 'z')
		{
			memcpy(summary, s, 1);
			summary++;
			s++;
		}
		else
		{
			memcpy(summary, " ", 1);
			summary++;
			s++;
		}
	}

	*summary = 0;
}

int context_ppl(dict_t *pstDict, char *url, char *title, char *context)
{
	int i,j,k,iRetCode;
	char *inputstr=NULL;
	char *summary=NULL;
	INDEXDATA node;

	inputstr = (char*)malloc(MAX_HTML_LEN);
	assert(inputstr);
	summary = (char*)malloc(MAX_HTML_LEN);
	assert(summary);
	
	snprintf(inputstr, MAX_HTML_LEN, "%s\n%s\n%s", url, title, context);

	pstDict->m_iCurrCount = pstDict->m_iWordCount;
	pstDict->m_iSentCount = pstDict->m_iSentTotal;

	iRetCode = cwlib_seg_word(pstDict, inputstr, strlen(inputstr),
			&(pstDict->m_pstWord), &(pstDict->m_iCurrCount),
			&(pstDict->m_pstSent), &(pstDict->m_iSentCount));

	if(0 == iRetCode)
	{
		k=0;
		if (pstDict->m_iCurrCount > 0)
		{
			qsort(pstDict->m_pstWord, pstDict->m_iCurrCount, sizeof(word_t), pword_compare);
			for(j=1; j<pstDict->m_iCurrCount; j++)
			{
				if(0 != pword_compare(&pstDict->m_pstWord[j], &pstDict->m_pstWord[k]))
				{
					if(++k != j)
					{
						memcpy(&pstDict->m_pstWord[k], &pstDict->m_pstWord[j], sizeof(word_t));
					}
				}
			}
		}
		k++;
		pstDict->m_iCurrCount = k;
	}
	else
	{
		pstDict->m_iCurrCount = 0;
		printf("ppl error!\n");
	}

	get_summary(context, summary);
	trim_sent(summary, context, MAX_HTML_LEN);

	bzero(&node, sizeof(INDEXDATA));
	strcpy(node.url, url);
	trim_sent(title, node.title, MAX_ITEM_LEN);
	node.insertTime = time(NULL);
	node.wordNum = pstDict->m_iCurrCount;
	node.summaryLen = strlen(context);

	tradenode_send(&node, pstDict->m_pstWord, context);

	free(inputstr);
	free(summary);

	return 0;
}

int main(int argc, char **argv)
{
	char filename[256],url[MAX_ITEM_LEN],title[MAX_ITEM_LEN];
	FILE *listfp,*txtfp;
	unsigned long long key;
	char *html,*context,*comphtml;
	html_entry htmlNode;

	int iRetCode;
	char *szPath = "../dict/";
	dict_t tDict = {0};

	if(argc != 2)
	{
		printf("%s listfile\n",argv[0]);
		exit(1);
	}
	
	iRetCode = dict_load(szPath, &tDict);
	if (0 != iRetCode) 
	{
		printf("dict_load(%s) failed\n", szPath);
		exit(1);
	}
	else
	{
		printf("dict_load OK!\n");
	}

	html = (char*)malloc(MAX_HTML_LEN);
	assert(html);
	context = (char*)malloc(MAX_HTML_LEN);
	assert(context);
	comphtml = (char*)malloc(MAX_HTML_LEN);
	assert(comphtml);
	
	mylinkExtract_init();

	neo_bdbenv_init("/home2/snapshot/");

	neo_bdb_init("/home2/snapshot/", DB_CREATE);
	printf("bdb open success.\n");

	listfp=fopen(argv[1], "r");
	assert(listfp);

	while(fgets(filename, 256, listfp))
	{
		filename[strlen(filename) - 1] = 0;
		printf("index...(%s)\n",filename);

		txtfp=fopen(filename, "r");
		assert(txtfp);

		while(get_one_html(txtfp, &htmlNode, url, title, html, comphtml) == 0)
		{
			key = url2id(url);
			assert(key);

			if(neo_bdb_bin_insert(&key, comphtml, htmlNode.m_iLenHtml) == 0)
			{
				//parse html
				mytextExtract_produce(url, html, strlen(html), title, context);

				context_ppl(&tDict, url, title, context);
			}
		}

		fclose(txtfp);
	}

	if(idxfp != NULL)
		fclose(idxfp);
	fclose(listfp);
	neo_bdb_close();

	free(html);
	free(context);
	free(comphtml);

	return 0;
}
