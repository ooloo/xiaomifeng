#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "nlpub.h"
#include "newdict.h"
#include "avl.h"
#include "base_log.h"
#include "timings.h"
#include "tradepub.h"
#include "Hootcp.h"

int sendSock=-1;
char *sendBuf=NULL;

int pword_compare(const void *pword1, const void *pword2)
{
	return strcoll(((word_t*)pword1)->m_szWord, ((word_t *)pword2)->m_szWord);
}

int tradenode_send(TRADEDATA *node, word_t *pword, int iword)
{
	int len=0;

	if(sendBuf == NULL)
	{
		sendBuf = (char*)malloc(sizeof(TRADEDATA) + sizeof(int) + MAX_WORD_NUM * sizeof(word_t));
		assert(sendBuf);
	}

	iword = iword <= MAX_WORD_NUM ? iword : MAX_WORD_NUM;

	memcpy(sendBuf, node, sizeof(TRADEDATA));
	len += sizeof(TRADEDATA);

	memcpy(sendBuf + len, &iword, sizeof(int));
	len += sizeof(int);

	memcpy(sendBuf + len, pword, iword * sizeof(word_t));
	len += iword * sizeof(word_t);

	printf("---------------------------------------\n");
	printf("%s\n", node->trLink);
	printf("%s\n", node->trTitle);
	printf("%s\n", node->trGame);
	printf("%f\n", node->trPrice);

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

void print_word(dict_t *pstDict, word_t *pword, int iword){
	FILE *fp = NULL;
	int i=0, j = 0;
	gameprop_t * pgameprop = NULL;
	int iprop = 0;
	fp = fopen(".//out.txt", "a");
	fprintf(fp, "total=%d\n", iword);
	for(i=0; i<iword; i++)
	{
		fprintf(fp, "[%s]\t\tgameprop=%-6d\tDF=%-5d\tTF=%-3d\tpos=%-2d\tinner=%d\tthes=%d(%s)\n",
				pword->m_szWord, pword->m_tProp.m_iIdxGame, pword->m_tProp.m_iDF, 
				pword->m_tProp.m_iTF, pword->m_tProp.m_tPos, pword->m_tProp.m_tInner, 
				pword->m_tProp.m_iIdxThes, (pstDict->m_pstThes+pword->m_tProp.m_iIdxThes)->m_szThes);
		if(0 != dict_prop_get(pstDict, pword, &pgameprop, &iprop))
		{
			printf("get prop error!\n");
		}
		for(j = 0; j < iprop; j ++)
		{
			fprintf(fp, "gameprop--> %lu\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
					pgameprop[j].m_tWid, pgameprop[j].m_tGame, pgameprop[j].m_tZy, pgameprop[j].m_tRw, pgameprop[j].m_tNpc,
					pgameprop[j].m_tDt, pgameprop[j].m_tJn, pgameprop[j].m_tGw, pgameprop[j].m_tWc, pgameprop[j].m_tFj,
					pgameprop[j].m_tDj, pgameprop[j].m_tSp, pgameprop[j].m_tOther);
		}
		pword++;
	}
	fclose(fp);
}

int read_idx12(FILE *idxfp, TRADEDATA *node)
{
	u_int8_t op;
	int i, ret=0;
	int fieldNum,fieldLen,fieldid,attr;
	unsigned long long id;
	char idxfilename[128];

	bzero(node, sizeof(TRADEDATA));

	if(fread(&id, 8, 1, idxfp) != 1
	   || fread(&op, 1, 1, idxfp) != 1
	   || fread(&fieldNum, 4, 1, idxfp) != 1)
	{
		return -1;
	}

	for(i=0; i<fieldNum; i++)
	{
		if(fread(&fieldid, 4, 1, idxfp) != 1
		   || fread(&attr, 4, 1, idxfp) != 1
		   || fread(&fieldLen, 4, 1, idxfp) != 1)
		{
			return -1;
		}

		switch(i)
		{
		case 0:
			ret = fread(node->trLink, fieldLen, 1, idxfp);
			break;
		case 1:
			ret = fread(node->trTitle, fieldLen, 1, idxfp);
			break;
		case 2:
			ret = fread(node->trGame, fieldLen, 1, idxfp);
			break;
		case 3:
			ret = fread(&node->trPrice, fieldLen, 1, idxfp);
			break;
		case 4:
			ret = fread(node->trMode, fieldLen, 1, idxfp);
			break;
		case 5:
			ret = fread(&node->trPubTime, fieldLen, 1, idxfp);
			break;
		case 6:
			ret = fread(node->trLeftTime, fieldLen, 1, idxfp);
			break;
		case 7:
			ret = fread(&node->trType, fieldLen, 1, idxfp);
			break;
		case 8:
			ret = fread(node->trUserRank, fieldLen, 1, idxfp);
			break;
		case 9:
			ret = fread(&node->trKind, fieldLen, 1, idxfp);
			break;
		case 10:
			ret = fread(&node->trNumber, fieldLen, 1, idxfp);
			break;
		case 11:
			ret = fread(node->trStat, fieldLen, 1, idxfp);
			break;
		case 12:
			ret = fread(node->trUserInfo, fieldLen, 1, idxfp);
			break;
		default:
			break;
		}
	}

	return 0;
}

int get_domain_word(char * url ,char *dm)
{
	int ret,len;
	char *t,*p;
	char domain[128];

	t = strstr(url, "://");
	if(t == NULL)
		return -1;
	else
		t += 3;

	p = t;
	while(*p)
	{
		if(*p == ':' || *p == '/')
			break;
		p++;
	}

	len = p - t;
	if(len >= 128)
		return -1;

	memcpy(domain, t, len);
	*(domain + len) = 0;
	
	ret = get_domainnames(domain, dm);
	if(ret == -1)
		return -1;

	p = strchr(dm, '.');
	if(p == NULL)
		return -1;
	else
		*p = 0;

	return 0;
}

int main(int argc, char** argv)
{
	int iRetCode = 0;
	int iword = 0, j = 0, k = 0;
	char *szPath = ".//dict";
	char inputstr[1024], dm[128];
	dict_t tDict = {0};
	dict_t *pstDict = &tDict;
	FILE *fp = NULL;
	struct timeval tvStart = {0}, tvEnd = {0}, tvTotal = {0};	
	TRADEDATA node;

	if(argc != 2)
	{
		printf("%s inputfile\n", argv[0]);
		exit(1);
	}

	if ((fp=fopen(argv[1], "rb")) == NULL)
	{
		printf("Open inputfile error!\n");
		return 0;
	}

	iRetCode = dict_load(szPath, &tDict);
	if (0 != iRetCode) 
	{
		printf("dict_load(%s) failed\n", szPath);
		return 0;
	}
	else
	{
		printf("dict_load OK!\n");
	}

	gettimeofday(&tvStart, NULL);
	while(read_idx12(fp, &node) == 0)
	{
		pstDict->m_iCurrCount = pstDict->m_iWordCount;
		pstDict->m_iSentCount = pstDict->m_iSentTotal;

		if(get_domain_word(node.trLink, dm) == -1)
			dm[0] = 0;

		snprintf(inputstr, 1024, "%s\t%s\t%s", dm, node.trTitle, node.trGame);

		iRetCode = cwlib_seg_word(pstDict, inputstr, strlen(inputstr),
								  &(pstDict->m_pstWord), &(pstDict->m_iCurrCount),
								  &(pstDict->m_pstSent), &(pstDict->m_iSentCount));

		if(ERR_OK == iRetCode) 
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

		tradenode_send(&node, pstDict->m_pstWord, pstDict->m_iCurrCount);
	}
	gettimeofday(&tvEnd, NULL);
	DIFF_TIME((&tvEnd), (&tvStart), (&tvTotal));
	printf("%s's time = %d %d\n", argv[0], k, tvTotal.tv_sec * 1000000L + tvTotal.tv_usec);

	fclose(fp);
	return 0;
}
