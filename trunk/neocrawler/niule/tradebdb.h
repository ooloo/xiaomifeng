#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "/usr/include/db4/db.h"

#define	MAX_DAT_LEN	1024
#define	DATABASE	"data.db"

unsigned str2key(const char *str);
unsigned long long url2id(const char *url);

int neo_bdbenv_init(char *dbdir);
int neo_bdb_init(char *dbdir, u_int32_t flags);

int neo_bdb_bin_insert(void *key, char *dat, int datLen);
int neo_bdb_bin_search(void *key, void *dat, int *datLen);
int neo_bdb_bin_search_t(void *key, char *dat, int *datLen);

int neo_bdb_insert(void *key, char *dat);
int neo_bdb_search(void *key, char *dat);
int neo_bdb_search_t(void *key, char *dat);

int neo_bdb_sync();

int neo_bdb_close();

