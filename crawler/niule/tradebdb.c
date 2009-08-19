#include <assert.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <openssl/md5.h>
#include "tradebdb.h"

#define MAX_BDB_NUM 64

DB_ENV *dbenv=NULL;
DB *dbp[MAX_BDB_NUM]={NULL};
pthread_mutex_t bdb_mutex[MAX_BDB_NUM];

unsigned str2key(const char *str)
{
	unsigned key[4];

	assert(MD5(str, strlen(str), (unsigned char*)key));

	return key[0];
}

unsigned long long url2id(const char *url)
{
	unsigned long long key[2];

	assert(MD5(url, strlen(url), (unsigned char*)key));

	return key[0];
}

unsigned get_bdb_num(void *signs)
{
	unsigned mod;

	mod = (*(unsigned*)signs)%MAX_BDB_NUM;

	return mod;
}

void init_DBT(DBT *key, DBT *data)
{
	memset(key, 0, sizeof(DBT));
	memset(data, 0, sizeof(DBT));
}

//return 1:found 0:not found 
int filename_check(char *filename)
{
	int ret;
	char *t,bdbname[128];
	DB *filedb=NULL;
	DBT key, data;

	if ((ret = db_create(&filedb, NULL, 0)) != 0) { 
		fprintf(stderr, "db_create: %s\n", db_strerror(ret)); 
		exit (1); 
	} 

	snprintf(bdbname, 128, "./done.filename.bdb");

	if ((ret = filedb->open(filedb, NULL, bdbname, NULL, DB_BTREE, DB_CREATE, 0664)) != 0)
	{ 
		filedb->err(filedb, ret, "%s", bdbname); 
		exit(1);
	}

	t = strrchr(filename, '/');
	if(t == NULL)
		t = filename;
	else
		t++;

	init_DBT(&key, &data);

	key.data = t;
	key.size = strlen(t);

	ret = filedb->get(filedb, NULL, &key, &data, 0);
	filedb->close(filedb, 0);

	if(ret == 0)
		return 1;
	else
		return 0;
}

int filename_insert(char *filename)
{
	int ret;
	char *t,bdbname[128];
	DB *filedb=NULL;
	DBT key, data;

	if ((ret = db_create(&filedb, NULL, 0)) != 0) { 
		fprintf(stderr, "db_create: %s\n", db_strerror(ret)); 
		exit (1); 
	} 

	assert(filedb->set_pagesize(filedb, 64*1024) == 0);

	snprintf(bdbname, 128, "./done.filename.bdb");

	if ((ret = filedb->open(filedb, NULL, bdbname, NULL, DB_BTREE, DB_CREATE, 0664)) != 0)
	{ 
		filedb->err(filedb, ret, "%s", bdbname); 
		exit(1);
	}

	t = strrchr(filename, '/');
	if(t == NULL)
		t = filename;
	else
		t++;

	init_DBT(&key, &data);

	key.data = t;
	key.size = strlen(t);

	ret = filedb->put(filedb, NULL, &key, &data, 0);
	filedb->close(filedb, 0);

	return 0;
}

int neo_bdbenv_init(char *dbdir)
{
	int i, ret;

	mkdir(dbdir, 0777);

	if ((ret = db_env_create(&dbenv, 0)) != 0) {
		fprintf(stderr, "env create %s\n", db_strerror(ret));
		exit(1);
	}

	if ((ret = dbenv->set_cachesize(dbenv, 0, 1 * 1024 * 1024, 0)) != 0) {
		fprintf(stderr, "set cachesize %s\n", db_strerror(ret));
		exit(1);
	}

	/* Open an environment with just a memory pool. */
	if ((ret = dbenv->open(dbenv, dbdir, DB_CREATE|DB_INIT_MPOOL|DB_INIT_CDB, 0)) != 0) {
		dbenv->err(dbenv, ret, "environment open error");
		exit(1);
	}

	for(i=0; i<MAX_BDB_NUM; i++)
	{
		pthread_mutex_init(&bdb_mutex[i],NULL);
	}

	return 0;
}

int neo_bdb_init(char *dbdir, u_int32_t flags)
{
	int i=0,ret;
	char bdbname[128];

	mkdir(dbdir, 0777);

	for(i=0; i<MAX_BDB_NUM; i++)
	{
		if ((ret = db_create(&dbp[i], dbenv, 0)) != 0) { 
			fprintf(stderr, "db_create: %s\n", db_strerror(ret)); 
			exit (1); 
		} 

		assert(dbp[i]->set_pagesize(dbp[i], 64*1024) == 0);

		snprintf(bdbname, 128, "%s.%03d", DATABASE, i);

		if ((ret = dbp[i]->open(dbp[i], NULL, bdbname, NULL, DB_BTREE, flags, 0664)) != 0)
		{ 
			dbp[i]->err(dbp[i], ret, "bdb open: %s", bdbname); 
			exit(1);
		}
	}

	return 0;
}

int neo_bdb_bin_insert(void *signs, char *dat, int datLen)
{
	int ret;
	unsigned mod;
	DBT key, data;
	
	init_DBT(&key, &data);

	key.data = signs; 
	key.size = sizeof(u_int64_t); 
	data.data = dat;
	//data.size = strlen(dat) + 1; 
	data.size = datLen; 

	mod = (*(unsigned*)signs)%MAX_BDB_NUM;

	pthread_mutex_lock(&bdb_mutex[mod]);
	ret = dbp[mod]->put(dbp[mod], NULL, &key, &data, DB_NOOVERWRITE);
	pthread_mutex_unlock(&bdb_mutex[mod]);
	switch (ret) 
	{ 
	case 0: 
		return 0; 
	default: 
		//dbp[mod]->err(dbp[mod], ret, "DB->put"); 
		return -1;
	}
}

int neo_bdb_bin_search_t(void *signs, char *dat, int *datLen)
{
	int ret;
	unsigned mod;
	char bdbname[128];
	DB *searchdbp;
	DBT key, data;

	mod = (*(unsigned*)signs)%MAX_BDB_NUM;

	if ((ret = db_create(&searchdbp, dbenv, 0)) != 0) { 
		fprintf(stderr, "db_create: %s\n", db_strerror(ret)); 
		exit (1); 
	} 

	snprintf(bdbname, 128, "%s.%03d", DATABASE, mod);

	if ((ret = searchdbp->open(searchdbp, NULL, bdbname, NULL, DB_BTREE, DB_RDONLY, 0664)) != 0)
	{ 
		searchdbp->err(searchdbp, ret, "%s", bdbname); 
		exit(1);
	}

	init_DBT(&key, &data);

	key.data = signs;
	key.size = sizeof(u_int64_t);

	ret = searchdbp->get(searchdbp, NULL, &key, &data, 0);
	switch (ret)
	{ 
	case 0:
		*datLen = data.size;
		memcpy(dat, (char*)data.data, data.size);
		searchdbp->close(searchdbp, 0);
		return 0; 
	default: 
		searchdbp->err(searchdbp, ret, "DB->get");
		searchdbp->close(searchdbp, 0);
		return -1;
	}
}

int neo_bdb_bin_search(void *signs, void *dat, int *datLen)
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
		*datLen = data.size;
		memcpy(dat, (char*)data.data, data.size);
		return 0; 
	default: 
		//dbp[mod]->err(dbp[mod], ret, "DB->get"); 
		return -1;
	}
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
	ret = dbp[mod]->put(dbp[mod], NULL, &key, &data, DB_NOOVERWRITE);
	pthread_mutex_unlock(&bdb_mutex[mod]);
	switch (ret) 
	{ 
	case 0: 
		return 0; 
	default: 
		//dbp[mod]->err(dbp[mod], ret, "DB->put"); 
		return -1;
	}
}

int neo_bdb_search_t(void *signs, char *dat)
{
	int ret;
	unsigned mod;
	char bdbname[128];
	DB *searchdbp;
	DBT key, data;

	mod = (*(unsigned*)signs)%MAX_BDB_NUM;

	if ((ret = db_create(&searchdbp, dbenv, 0)) != 0) { 
		fprintf(stderr, "db_create: %s\n", db_strerror(ret)); 
		exit (1); 
	} 

	snprintf(bdbname, 128, "%s.%03d", DATABASE, mod);

	if ((ret = searchdbp->open(searchdbp, NULL, bdbname, NULL, DB_BTREE, DB_RDONLY, 0664)) != 0)
	{ 
		searchdbp->err(searchdbp, ret, "%s", bdbname); 
		exit(1);
	}

	init_DBT(&key, &data);

	key.data = signs;
	key.size = sizeof(u_int64_t);

	ret = searchdbp->get(searchdbp, NULL, &key, &data, 0);
	switch (ret)
	{ 
	case 0:
		snprintf(dat, MAX_DAT_LEN, "%s", (char*)data.data);
		searchdbp->close(searchdbp, 0);
		return 0; 
	default: 
		searchdbp->err(searchdbp, ret, "DB->get");
		searchdbp->close(searchdbp, 0);
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
		snprintf(dat, MAX_DAT_LEN, "%s", (char*)data.data);
		return 0; 
	default: 
		//dbp[mod]->err(dbp[mod], ret, "DB->get"); 
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
		if(dbp[i] != NULL)
		{
			dbp[i]->close(dbp[i], 0);
			dbp[i] = NULL;
		}
	}

	return 0;
}

