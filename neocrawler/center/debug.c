#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <strings.h>
#include <time.h>
#include "centerbdb.h" 
#include "centerCache.h"

int main(int argc, char **argv)
{
	char ip[32],dat[2048],url[1024];
	unsigned char key[16];
	unsigned long long sign;
	URLNODE_T urlNode;

	if(argc != 2)
	{
		printf("%s url\n",argv[0]);
		exit(1);
	}

	neo_bdb_init(DB_RDONLY);
	printf("bdb open success.\n");
	
	if(strncmp(argv[1], "http://", 7) != 0)
	{
		sscanf(argv[1], "%llu", &sign);
		memcpy(key, &sign, 8);
		printf("key: %llu\n", *(unsigned long long*)key);
	}
	else
	{
		assert(MD5(argv[1], strlen(argv[1]), (unsigned char*)key));
		printf("key: %llu\n", *(unsigned long long*)key);
	}

	if(neo_bdb_search(&key, dat) == 0)
	{
#ifndef RADAR
		printf("----------------------------------------------------\n");
		printf("%s\n", dat);
		printf("----------------------------------------------------\n");
		printf("thanks, Bye...\n");
		return 0;
#endif
		//printf("%s\n", dat);
		sscanf(dat, "%d %d %u %u %u %u %u %u %u %u %u %u %u %s %s",
				 &urlNode.insertTime,
				 &urlNode.updateTime,
				 &urlNode.urlType,
				 &urlNode.urlStat,
				 &urlNode.urlRank,
				 &urlNode.urlOther,
				 &urlNode.urlLen,
				 &urlNode.linkNum,
				 &urlNode.sendNum,
				 &urlNode.statusCode,
				 &urlNode.bbsMysqlId,
				 &urlNode.linkPickInfo,
				 &urlNode.htmlLen,
				 url,
				 ip);

		printf("----------------------------------------------------\n");
		printf("urlNode.insertTime	= %s", ctime((time_t*)&urlNode.insertTime));
		printf("urlNode.updateTime	= %s", ctime((time_t*)&urlNode.updateTime));
		printf("urlNode.urlType		= %d\n", urlNode.urlType);
		printf("urlNode.urlStat		= %d\n", urlNode.urlStat);
		printf("urlNode.urlRank		= %d\n", urlNode.urlRank);
		printf("urlNode.urlOther	= %d\n", urlNode.urlOther);
		printf("urlNode.urlLen		= %d\n", urlNode.urlLen);
		printf("urlNode.linkNum		= %d\n", urlNode.linkNum);
		printf("urlNode.sendNum		= %d\n", urlNode.sendNum);
		printf("urlNode.statusCode	= %d\n", urlNode.statusCode);
		printf("urlNode.bbsMysqlId	= %d\n", urlNode.bbsMysqlId);
		printf("urlNode.linkPickInfo	= %d\n", urlNode.linkPickInfo);
		printf("urlNode.htmlLen		= %d\n", urlNode.htmlLen);
		printf("urlNode.url		= %s\n", url);
		printf("target ip		= %s\n", ip);
		printf("----------------------------------------------------\n");
		printf("thanks, Bye...\n");
	}

	//neo_bdb_close();
	return 0;
}
