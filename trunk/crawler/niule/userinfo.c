#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <pthread.h>
#include <openssl/md5.h>
#include "hashtable.h"
#include "Hootcp.h"
#include "neopub.h"

#define HASH_SLOT_NUM	5000011

TDict *userinfo_dict = NULL;

pthread_mutex_t dict_mutex=PTHREAD_MUTEX_INITIALIZER;

void userinfo_insert(TDict *dict, char *keyStr)
{
	unsigned slotNo,key;
	TNodeInfo info;

	assert(dict && keyStr);

	hashtable_hash(dict, &slotNo, &key, keyStr);

	pthread_mutex_lock(&dict_mutex);
	assert(hashtable_insert(dict,slotNo,key,&info) != -1);
	pthread_mutex_unlock(&dict_mutex);

	return;
}

// return 1:found 0:notfound 
int userinfo_search(TDict *dict, char *keyStr)
{   
	int ret; 
	unsigned slotNo,key;
	TNodeInfo info;

	assert(dict && keyStr);

	hashtable_hash(dict, &slotNo, &key, keyStr);

	pthread_mutex_lock(&dict_mutex);
	ret = hashtable_seek(dict,slotNo,key,&info);
	pthread_mutex_unlock(&dict_mutex);

	return ret;
}

int userinfo_recv_thr()
{
	int len, bufLen;
	int recvSock=-1, recvListenSock=-1;
	char recvBuf[MAX_ITEM_LEN];

	recvListenSock = htcp_listen(9727, 5);
	assert(recvListenSock);
	printf("recv htcp_listen success [%d].\n", 9727);

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

		while(htcp_recv(recvSock, recvBuf, MAX_ITEM_LEN) != -1)
		{
			userinfo_insert(userinfo_dict, recvBuf);
		}

		close(recvSock);
	}
	return 0;
}

void userinfo_search_thr(int searchSock)
{
	char recvBuf[MAX_ITEM_LEN];

	pthread_detach(pthread_self());

	while(1)
	{
		if(htcp_recv(searchSock, recvBuf, MAX_ITEM_LEN) != -1)
		{
			break;
		}
		
		if(userinfo_search(userinfo_dict, recvBuf) == 0)
			*recvBuf = 0;
		else
			*recvBuf = 1;
		
		if(htcp_send(searchSock, recvBuf, 1) != -1)
		{
			break;
		}
	}

	close(searchSock);
}

void userinfo_search_accept()
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

		if(pthread_create(&id, NULL, (void *)userinfo_search_thr, (void *)searchSock) != 0)
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

	assert(userinfo_dict=hashtable_init(HASH_SLOT_NUM));

	if(pthread_create(&id, NULL, (void *)userinfo_recv_thr, NULL) != 0)
	{
		printf("create trade_recv_thr error.\n");
		exit(1);
	}

	if(pthread_create(&id, NULL, (void *)userinfo_search_accept, NULL) != 0)
	{
		printf("create trade_recv_thr error.\n");
		exit(1);
	}

	pthread_join(id, NULL);

	return 0;
}
