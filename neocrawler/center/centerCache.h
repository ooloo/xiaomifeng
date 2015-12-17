#ifndef _CENTER_CACHE_H
#define _CENTER_CACHE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <openssl/md5.h>
#include "rbdict.h"
#include "Hootcp.h"

#define MAX_RANK 256
#define MIN_RANK (MAX_RANK - 250)
#define MAX_URL_LEN 1024
#define MAX_DOMAIN_LEN 64

typedef enum {
    NORMAL = 0,
    INDEX = 1,
    MANUAL = 2,
} URLTYPE;

typedef enum {
    INSERT = 0,
    UPDATE = 1,
    DELETE = 2,
    UNCHANGE = 3,
} URLSTAT;

typedef enum {
    BBSNOTIME = 12,
    BBSPUBTIME = 13,
} LINKPICKINFO;

typedef u_int64_t KEY_T;

typedef struct _Dnode {
    unsigned short domainType;
    unsigned short domainRank;
    unsigned insertCount;
    unsigned updateCount;
    char domain[MAX_DOMAIN_LEN];
    TRbDict *urlDict;
    TRbIterator *updateItor;
} DOMAINNODE_T;

typedef struct _Unode {
    int insertTime;
    int updateTime;
    u_int8_t urlType;
    u_int8_t urlStat;
    u_int8_t urlRank;
    u_int8_t urlOther;
    unsigned short urlLen;
    unsigned short linkNum;
    unsigned short sendNum;
    unsigned short statusCode;
    unsigned short bbsMysqlId;
    unsigned short linkPickInfo;
    unsigned htmlLen;
} URLNODE_T;

extern TRbDict *g_domainRBdict;
extern int g_basePort;
extern char dumpfile[];
extern pthread_mutex_t cache_mutex;

int key_cmp(const void *key1, const void *key2);

#endif
