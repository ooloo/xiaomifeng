extern "C"{
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>

#define	MAX_URL_LEN	1024
#define	DBDIR		"./indexbdb/"
#define	DATABASE	"snapshot.db"

unsigned long long url2id(const char *url);

int neo_bdb_init();

int neo_bdb_insert(void *signs, char *dat);

int neo_bdb_search(void *signs, char *dat);

int neo_bdb_sync();

int neo_bdb_close();
}

