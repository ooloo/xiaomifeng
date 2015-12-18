#ifndef _IP_CACHE_H
#define _IP_CACHE_H

#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <openssl/md5.h>
#include <pthread.h>
#include <netdb.h>
#include "rbdict.h"

#define MAX_IP_NUM 8

TRbDict *g_ipRBdict = NULL;
pthread_mutex_t ip_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef u_int64_t KEY_T;

typedef struct _ipnode {
    unsigned pos;
    unsigned long addr[MAX_IP_NUM];
    int refreshTime;
} IPNODE_T;

int key_cmp(const void *key1, const void *key2)
{
    if (*(KEY_T *) key1 > *(KEY_T *) key2)
        return 1;
    else if (*(KEY_T *) key1 == *(KEY_T *) key2)
        return 0;
    else
        return -1;
}

void key_del(void *node)
{
    free(node);

    return;
}

void update_ip(char *host, IPNODE_T * node)
{
    int i, ret, herr;
    char addrBuf[1024];
    struct hostent *he, tmphe;

    bzero(node, sizeof(IPNODE_T));
    node->refreshTime = time(NULL);

    ret = gethostbyname_r(host, &tmphe, addrBuf, 1024, &he, &herr);
    if (ret == 0 && he != NULL)
    {
        for (i = 0; i < MAX_IP_NUM; i++)
        {
            if (he->h_addr_list[i] != NULL)
                node->addr[i] = *(unsigned long *) he->h_addr_list[i];
            else
                break;
        }
    } else
    {
        for (i = 0; i < MAX_IP_NUM; i++)
        {
            node->addr[i] = INADDR_NONE;
        }
    }

    return;
}

unsigned long ip_search(char *host)
{
    unsigned i, signs[4];
    unsigned long ip = 0;
    void *tmp;
    IPNODE_T *node;

    if (g_ipRBdict == NULL)
        return 0;

    assert(MD5(host, strlen(host), (unsigned char *) signs));

    pthread_mutex_lock(&ip_mutex);
    if (rb_dict_search(g_ipRBdict, signs, &tmp) == 0)
    {
        node = (IPNODE_T *) tmp;
        if (time(NULL) - node->refreshTime > 86400)
        {
            ip = 0;
        } else
        {
            if (node->pos >= MAX_IP_NUM)
                node->pos = 0;

            i = node->pos++;
            if (node->addr[i] == 0)
            {
                node->pos = 0;
                ip = node->addr[0];
            } else
            {
                ip = node->addr[i];
            }
        }
    }
    pthread_mutex_unlock(&ip_mutex);

    return ip;
}

unsigned long ip_insert(char *host)
{
    unsigned signs[4];
    void *tmp;
    IPNODE_T *node;

    pthread_mutex_lock(&ip_mutex);
    if (g_ipRBdict == NULL)
    {
        g_ipRBdict = rb_dict_new((void *) key_cmp, (void *) key_del, NULL);
        assert(g_ipRBdict);
    }
    pthread_mutex_unlock(&ip_mutex);

    assert(MD5(host, strlen(host), (unsigned char *) signs));

    tmp = (void *) malloc(sizeof(KEY_T) + sizeof(IPNODE_T));
    assert(tmp);

    memcpy(tmp, signs, sizeof(KEY_T));
    node = (IPNODE_T *) (tmp + sizeof(KEY_T));

    update_ip(host, node);

    pthread_mutex_lock(&ip_mutex);
    rb_dict_insert(g_ipRBdict, tmp, tmp + sizeof(KEY_T), 1);
    pthread_mutex_unlock(&ip_mutex);

    return node->addr[0];
}

unsigned long get_ip(char *host)
{
    unsigned long ip;

    ip = ip_search(host);
    if (ip == 0)
        ip = ip_insert(host);
    //else
    //      printf("search ip: %u\n",ip);

    if (ip == INADDR_NONE)
        return 0;
    else
        return ip;

    return 0;
}

#endif
