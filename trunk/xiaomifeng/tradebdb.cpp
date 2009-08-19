extern "C" {
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <openssl/md5.h>
#include <db.h>  
#include "tradebdb.h"

#define MAX_BDB_NUM 128

unsigned cacheLinkNum=0;
DB *dbp[MAX_BDB_NUM]={NULL};
pthread_mutex_t bdb_mutex[MAX_BDB_NUM];

unsigned long long url2id(const char *url)
{
	unsigned long long key[2];

	assert(MD5((const unsigned char*)url, strlen(url), (unsigned char*)key));

	return key[0];
}

void init_DBT(DBT *key, DBT *data)
{
	memset(key, 0, sizeof(DBT));
	memset(data, 0, sizeof(DBT));
}

int neo_bdb_init()
{
	int i=0,ret;
	char bdbname[128];

	mkdir(DBDIR, 0777);

	for(i=0; i<MAX_BDB_NUM; i++)
	{
		if ((ret = db_create(&dbp[i], NULL, 0)) != 0) { 
			fprintf(stderr, "db_create: %s\n", db_strerror(ret)); 
			exit (1); 
		} 

		snprintf(bdbname, 128, "%s/%s.%03d", DBDIR, DATABASE, i);

		if ((ret = dbp[i]->open(dbp[i], NULL, bdbname, NULL, DB_BTREE, DB_CREATE, 0664)) != 0)
		{ 
			dbp[i]->err(dbp[i], ret, "%s", bdbname); 
			exit(1);
		}

		pthread_mutex_init(&bdb_mutex[i],NULL);
	}

	return 0;
}

int neo_bdb_insert(void *signs, char *dat)
{
	int ret;
	unsigned mod;
	DBT key, data;
	
	init_DBT(&key, &data);

	key.data = signs; 
	key.size = sizeof(u_int64_t); 
	data.data = dat;
	data.size = strlen(dat) + 1; 

	mod = (*(unsigned*)signs)%MAX_BDB_NUM;

	pthread_mutex_lock(&bdb_mutex[mod]);
	ret = dbp[mod]->put(dbp[mod], NULL, &key, &data, 0);
	pthread_mutex_unlock(&bdb_mutex[mod]);
	switch (ret) 
	{ 
	case 0: 
		return 0; 
	default: 
		dbp[mod]->err(dbp[mod], ret, "DB->put"); 
		return -1;
	} 
}

int neo_bdb_search(void *signs, char *dat)
{
	int ret;
	unsigned mod;
	DBT key, data;

	init_DBT(&key, &data);

	key.data = signs;
	key.size = sizeof(u_int64_t);

	mod = (*(unsigned*)signs)%MAX_BDB_NUM;

	pthread_mutex_lock(&bdb_mutex[mod]);
	ret = dbp[mod]->get(dbp[mod], NULL, &key, &data, 0);
	pthread_mutex_unlock(&bdb_mutex[mod]);
	switch (ret) 
	{ 
	case 0:
		snprintf(dat, MAX_URL_LEN, "%s", (char*)data.data);
		return 0; 
	default: 
		dbp[mod]->err(dbp[mod], ret, "DB->get"); 
		return -1;
	}
}

int neo_bdb_sync()
{
	int i;

	for(i=0; i<MAX_BDB_NUM; i++)
	{
		pthread_mutex_lock(&bdb_mutex[i]);
		dbp[i]->sync(dbp[i], 0);
		pthread_mutex_unlock(&bdb_mutex[i]);
	}

	return 0;
}

int neo_bdb_close()
{
	int i;

	for(i=0; i<MAX_BDB_NUM; i++)
	{
		pthread_mutex_lock(&bdb_mutex[i]);
		dbp[i]->close(dbp[i], 0);
		pthread_mutex_unlock(&bdb_mutex[i]);
	}

	return 0;
}
}

