#include <assert.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <db.h>
#include "centerbdb.h"

int cacheNum[MAX_BDB_NUM]={0};
DB *dbp[MAX_BDB_NUM]={NULL};
pthread_mutex_t bdb_mutex[MAX_BDB_NUM];


void init_DBT(DBT *key, DBT *data)
{
	memset(key, 0, sizeof(DBT));
	memset(data, 0, sizeof(DBT));
}

void cmd_thr()
{
	int i,mod,syncTime;
	char stopfile[16]="/tmp/bdb.stop";
	FILE *fp;

	unlink(stopfile);

	syncTime = time(NULL);

	while(1)
	{
		sleep(60);

		if((fp=fopen(stopfile, "r")) != NULL)
		{
			fclose(fp);

			for(i=0; i<MAX_BDB_NUM; i++)
				pthread_mutex_lock(&bdb_mutex[i]);

			neo_bdb_close();
			exit(1);
		}

		if(time(NULL) - syncTime > 1800)
		{
			syncTime = time(NULL);

			for(mod=0; mod<MAX_BDB_NUM; mod++)
			{
				pthread_mutex_lock(&bdb_mutex[mod]);
				dbp[mod]->sync(dbp[mod], 0);
				pthread_mutex_unlock(&bdb_mutex[mod]);
			}
		}
	}

	return;
}

int neo_bdb_init(u_int32_t flags)
{
	int i=0,ret;
	char bdbname[128];
	pthread_t id;

	mkdir(DBDIR, 0777);

	for(i=0; i<MAX_BDB_NUM; i++)
	{
		if ((ret = db_create(&dbp[i], NULL, 0)) != 0)
		{ 
			fprintf(stderr, "db_create: %s\n", db_strerror(ret)); 
			exit (1); 
		} 

		assert(dbp[i]->set_pagesize(dbp[i], 64*1024) == 0);

		snprintf(bdbname, 128, "%s/%s.%03d", DBDIR, DATABASE, i);

		if ((ret = dbp[i]->open(dbp[i], NULL, bdbname, NULL, DB_BTREE, flags, 0664)) != 0)
		{ 
			dbp[i]->err(dbp[i], ret, "%s", bdbname); 
			exit(1);
		}

		pthread_mutex_init(&bdb_mutex[i],NULL);
	}

	ret = pthread_create(&id, NULL, (void *)cmd_thr, NULL);
	if(ret != 0)
	{
		printf("create cmd_thr thr error.\n");
		exit(1);
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
#ifndef RADAR
	ret = dbp[mod]->put(dbp[mod], NULL, &key, &data, DB_NOOVERWRITE);
#else
	ret = dbp[mod]->put(dbp[mod], NULL, &key, &data, 0);
#endif
	pthread_mutex_unlock(&bdb_mutex[mod]);

	switch (ret)
	{ 
	case 0:
		pthread_mutex_lock(&bdb_mutex[mod]);
		cacheNum[mod]++;
#ifndef RADAR
		if(cacheNum[mod] % 64 == 0)
#else
		if(cacheNum[mod] % 128 == 0)
#endif
		{
			dbp[mod]->sync(dbp[mod], 0);
		}
		pthread_mutex_unlock(&bdb_mutex[mod]);
		return 0;
	case DB_KEYEXIST:
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
		//dbp[mod]->err(dbp[mod], ret, "DB->get"); 
		return -1;
	} 
}

int neo_bdb_close()
{
	int i;
	
	for(i=0; i<MAX_BDB_NUM; i++)
		dbp[i]->close(dbp[i], 0);

	return 0;
}

