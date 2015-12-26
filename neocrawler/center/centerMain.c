#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <strings.h>
#include "centerCache.h"
#include "centerSelector.h"
#include "centerbdb.h"

#define MAX_FILTER_NUM 320000

int recvListenSock = -1;
int recvSock = -1;
int guideRecvSocket = -1;
int guideSendSocket = -1;
int g_basePort = 0;
char guideHost[128];

int guide_recv(void *buf, int len)
{
    assert(buf && len && len > sizeof(URLNODE_T));

    while (htcp_recv(guideRecvSocket, buf, len) == -1)
    {
        printf("guide recv error, socker=%d\n", guideRecvSocket);
        close(guideRecvSocket);
        guideRecvSocket = -1;

        sleep(1);
        while ((guideRecvSocket =
                htcp_connect(guideHost, g_basePort + 46)) == -1)
        {
            sleep(1);
            printf("htcp_connect guideRecvSocket error.\n");
        }
        printf("htcp_connect guideRecvSocket success.\n");
    }

    return 0;
}

int guide_send(URLNODE_T * urlNode, char *url)
{
    int len;
    char *p, buf[2048];

    assert(urlNode && url);

    urlNode->urlLen = strlen(url);

    len = sizeof(URLNODE_T) + urlNode->urlLen;
    assert(len < 2048);

    memcpy(buf, urlNode, sizeof(URLNODE_T));
    memcpy(buf + sizeof(URLNODE_T), url, urlNode->urlLen);

    while (htcp_send(guideSendSocket, buf, len) == -1)
    {
        printf("guide send error, socker=%d\n", guideSendSocket);
        close(guideSendSocket);
        guideSendSocket = -1;

        sleep(1);
        while ((guideSendSocket =
                htcp_connect(guideHost, g_basePort + 47)) == -1)
        {
            sleep(1);
            printf("htcp_connect guideSendSocket error.\n");
        }
        printf("htcp_connect guideSendSocket success.\n");
    }

    return 0;
}

void guide_work_thr()
{
    int gsocket = -1;
    long memoryPageSize, maxCahceNum;
    char buf[2048], url[MAX_URL_LEN];
    URLNODE_T *urlNode;

    urlNode = (URLNODE_T *) buf;
    assert((memoryPageSize = sysconf(_SC_PHYS_PAGES)) > 0);
    maxCahceNum = memoryPageSize * 48;

    while (1)
    {
        if (total_count > maxCahceNum)
        {
            sleep(5);
            if (guideRecvSocket > 0)
            {
                close(guideRecvSocket);
                guideRecvSocket = -1;
            }
            continue;
        }

        guide_recv(buf, 2048);

        if (urlNode->urlLen > MAX_URL_LEN)
        {
            printf("urlNode->urlLen error.\n");
            continue;
        }

        *(buf + sizeof(URLNODE_T) + urlNode->urlLen) = 0;
        snprintf(url, MAX_URL_LEN, "%s", buf + sizeof(URLNODE_T));
        printf("radar recv success. (%s)\n", url);

        //接受到新的url, 入库
        pthread_mutex_lock(&cache_mutex);
        NEO_cache_url(g_domainRBdict, url, urlNode);
        pthread_mutex_unlock(&cache_mutex);
    }

    return;
}

int node_recv(void *buf, int len)
{
    assert(buf && len);

    while (htcp_recv(recvSock, buf, len) == -1)
    {
        sleep(1);
        close(recvSock);

        if (recvListenSock == -1)
        {
            recvListenSock = htcp_listen(g_basePort + 1, 5);
            assert(recvListenSock);
        }
        printf("recv htcp_listen success [%d].\n", g_basePort + 1);

        while ((recvSock = htcp_accept(recvListenSock, NULL, NULL)) == -1)
        {
            sleep(1);
            printf("recv htcp_accept error.\n");
        }
        printf("recv htcp_accept success.\n");
    }

    return 0;
}

void cache_work_thr()
{
    unsigned ret;
    char buf[2048], url[MAX_URL_LEN];
    URLNODE_T *urlNode;

    urlNode = (URLNODE_T *) buf;

    while (1)
    {
        node_recv(buf, 2048);

        if (urlNode->urlLen > MAX_URL_LEN)
        {
            printf("urlNode->urlLen error.\n");
            continue;
        }

        *(buf + sizeof(URLNODE_T) + urlNode->urlLen) = 0;
        snprintf(url, MAX_URL_LEN, "%s", buf + sizeof(URLNODE_T));

        if ((ret = NEO_cache_check(g_domainRBdict, url)) > 100000)
        {
            printf("this domain save too many (%d) url.\n", ret);
            continue;
        }
        //本机url, 直接更新, 同时发送到radar记录
        guide_send(urlNode, url);

        if (urlNode->updateTime > 0)
        {
            pthread_mutex_lock(&cache_mutex);
            NEO_cache_url(g_domainRBdict, url, urlNode);
            pthread_mutex_unlock(&cache_mutex);
        }
    }

    return;
}

long fsize(char *filename)
{
    long len;
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL)
        return -1;
    else
    {
        assert(fseek(fp, 0L, SEEK_END) == 0);
        len = ftell(fp);
        fclose(fp);

        return len;
    }
}

int load_file(char *manualFile)
{
    int filterNum = 0;
    FILE *fp = NULL;
    char src[MAX_URL_LEN], url[MAX_URL_LEN], domain[128];
    KEY_T domainKey, urlKey, *signs;
    URLNODE_T urlNode;

    signs = (KEY_T *) calloc(MAX_FILTER_NUM, sizeof(KEY_T));
    assert(signs);

    bzero(&urlNode, sizeof(URLNODE_T));
    urlNode.urlType = MANUAL;

    if (manualFile != NULL)
    {
        fp = fopen(manualFile, "r");
        assert(fp);

        while (fgets(src, MAX_URL_LEN, fp))
        {
            sscanf(src, "%hd %s\n", &urlNode.bbsMysqlId, url);
            pthread_mutex_lock(&cache_mutex);
            NEO_cache_domain(g_domainRBdict, url, &urlNode);
            pthread_mutex_unlock(&cache_mutex);
        }

        fclose(fp);
    }

    if (fsize(dumpfile) < fsize("dumpfile.111"))
    {
        printf("dumpfile error.\n");
        free(signs);
        exit(1);
    }

    fp = fopen("filter.domain", "r");
    if (fp != NULL)
    {
        while (fgets(src, 256, fp) && filterNum < MAX_FILTER_NUM - 2)
        {
            if (get_domain(src, domain, 128) == -1)
                continue;
            else
                assert(MD5
                       (domain, strlen(domain),
                        (unsigned char *) (signs + filterNum++)));
            printf("filter domain: %llu %s\n", signs[filterNum - 1],
                   domain);
        }
        fclose(fp);

        qsort(signs, filterNum, sizeof(KEY_T), key_cmp);
    }

    fp = fopen(dumpfile, "rb");
    if (fp == NULL)
    {
        free(signs);
        return 0;
    }

    while (fread(&domainKey, sizeof(KEY_T), 1, fp) == 1)
    {
        fread(&urlKey, sizeof(KEY_T), 1, fp);
        fread(&urlNode, sizeof(URLNODE_T), 1, fp);

        if (filterNum > 0)
        {
            if (bsearch
                (&domainKey, signs, filterNum, sizeof(KEY_T),
                 key_cmp) != NULL)
            {
                printf("filter domain: %llu\n", domainKey);
                continue;
            }
        }

        if (urlNode.urlLen > 1024 || urlNode.htmlLen > 512 * 1024)
        {
            printf("Error!!!: %d %d\n", urlNode.urlLen, urlNode.htmlLen);
            continue;
        }
#ifdef BBS
        if (urlNode.updateTime > 0 && urlNode.urlType == NORMAL)
        {
            continue;
        }
#endif

        // rrrrrrrrrrrrrrrrrrre do
        //      if(neo_bdb_search(&urlKey, url) == 0)
        //      {
        //              printf("node send -> (%s), urlStat = %d\n", url, urlNode.urlStat);
        //              guide_send(&urlNode, url);
        //      }
        //      continue;
        // rrrrrrrrrrrrrrrrrrre do

        pthread_mutex_lock(&cache_mutex);
        NEO_cache_load(g_domainRBdict, &domainKey, &urlKey, &urlNode);
        pthread_mutex_unlock(&cache_mutex);
    }

    fclose(fp);
    free(signs);

    // rrrrrrrrrrrrrrrrrrre do
    //exit(1);
    // rrrrrrrrrrrrrrrrrrre do
    return 0;
}

int main(int argc, char **argv)
{
    int ret;
    FILE *fp;
    pthread_t cacheid, updateid, fetchid, logid, dumpid;

    if (argc < 3 || argc > 4)
    {
        printf("%s baseport host [bbs.mysql.file]\n", argv[0]);
        exit(1);
    }

    neo_bdb_init(DB_CREATE);
    printf("bdb open success.\n");

    NEO_cache_init();
    printf("cache init success.\n");

    g_basePort = atoi(argv[1]);
    assert(g_basePort > 1024 && g_basePort < 65535);

    strcpy(guideHost, argv[2]);
    if (argc == 4)
        load_file(argv[3]);
    else
        load_file(NULL);
    printf("load data success.\n");

    ret = pthread_create(&cacheid, NULL, (void *) guide_work_thr, NULL);
    if (ret != 0)
    {
        printf("create guide_work_thr thr error.\n");
        exit(1);
    }

    ret = pthread_create(&cacheid, NULL, (void *) cache_work_thr, NULL);
    if (ret != 0)
    {
        printf("create cache_work_thr thr error.\n");
        exit(1);
    }

    ret = pthread_create(&updateid, NULL, (void *) select_update_thr, NULL);
    if (ret != 0)
    {
        printf("create select_update_thr thr error.\n");
        exit(1);
    }

    ret = pthread_create(&fetchid, NULL, (void *) fetch_url_thr, NULL);
    if (ret != 0)
    {
        printf("create fetch_url_thr thr error.\n");
        exit(1);
    }

    ret = pthread_create(&logid, NULL, (void *) log_thr, NULL);
    if (ret != 0)
    {
        printf("create log_thr thr error.\n");
        exit(1);
    }

    ret = pthread_create(&dumpid, NULL, (void *) dump_thr, NULL);
    if (ret != 0)
    {
        printf("create dump_thr thr error.\n");
        exit(1);
    }

    pthread_join(cacheid, NULL);
    pthread_join(updateid, NULL);
    pthread_join(logid, NULL);
    pthread_join(dumpid, NULL);

    return 0;
}
