#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>

#define	MAX_URL_LEN	1024

#ifndef RADAR
#define	DBDIR		"./linkcenter/"
#define	DATABASE	"linkcenter.db"
#define MAX_BDB_NUM 64 
#else
#define	DBDIR		"./radarbdb/"
#define	DATABASE	"radar.db"
#define MAX_BDB_NUM 64
#endif

int neo_bdb_init(u_int32_t flags);

int neo_bdb_insert(void *key, char *dat);

int neo_bdb_search(void *key, char *dat);

