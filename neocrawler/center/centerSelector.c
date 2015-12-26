#include <assert.h>
#include <stdio.h>
#include <netdb.h>
#include "centerCache.h"

int sendSock = -1;
int sendListenSock = -1;
int total_count = 0;
int dumpCount = 0;
FILE *logfp = NULL;
FILE *dumpfp = NULL;
KEY_T *dumpDomainKey = NULL;
pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;

int node_send(URLNODE_T * urlNode, char *url)
{
    int len;
    char buf[2048];

    assert(urlNode);
    if (urlNode->urlLen != strlen(url))
        return 0;

    len = sizeof(URLNODE_T) + urlNode->urlLen;
    assert(len < 2048);

    memcpy(buf, urlNode, sizeof(URLNODE_T));
    memcpy(buf + sizeof(URLNODE_T), url, urlNode->urlLen);

    pthread_mutex_lock(&send_mutex);
    while (htcp_send(sendSock, buf, len) == -1)
    {
        sleep(1);
        close(sendSock);

        if (sendListenSock == -1)
        {
            sendListenSock = htcp_listen(g_basePort, 5);
            assert(sendListenSock);
        }
        printf("send htcp_listen success [%d].\n", g_basePort);

        while ((sendSock = htcp_accept(sendListenSock, NULL, NULL)) == -1)
        {
            printf("send accept error.\n");
            sleep(1);
        }
        printf("send htcp_accept success.\n");
    }
    pthread_mutex_unlock(&send_mutex);

    return 0;
}

int visit(const void *key, void *dat, void *args)
{
    char url[MAX_URL_LEN];
    KEY_T tmpKey, *urlKey = NULL;
    URLNODE_T tmpNode, *urlNode = NULL;
    TRbIterator *itor;

    bzero(&tmpKey, sizeof(KEY_T));
    bzero(&tmpNode, sizeof(URLNODE_T));

    if (strcmp((char *) args, "UPDATE") == 0)
    {
        usleep(5000);

        pthread_mutex_lock(&cache_mutex);
        itor = ((DOMAINNODE_T *) dat)->updateItor;
        assert(itor);

        while (rb_itor_next(itor))
        {
            urlNode = (URLNODE_T *) rb_itor_data(itor);
            urlKey = (KEY_T *) rb_itor_key(itor);
            if (urlNode != NULL && urlKey != NULL)
            {
                // 10 mins - 30 mins
                if (urlNode->urlType == MANUAL)
                {
                    if (urlNode->urlRank > MAX_RANK - 10)
                    {
                        urlNode->urlRank = MAX_RANK - 10;
                    } else if (urlNode->urlRank < MAX_RANK - 30)
                    {
                        urlNode->urlRank = MAX_RANK - 30;
                    }
                }

                if (urlNode->updateTime <= 0)
                {
                    ((DOMAINNODE_T *) dat)->insertCount++;
                    urlNode->updateTime = time(NULL);
                    urlNode->sendNum++;
                    break;
                }
                else if ((time(NULL) - urlNode->updateTime >
                          (MAX_RANK - urlNode->urlRank) * 60
                          && urlNode->urlType == MANUAL)
                         || (time(NULL) - urlNode->updateTime >
                             (MAX_RANK - urlNode->urlRank) * 120000
                             && urlNode->urlType == INDEX)
                         && urlNode->urlRank >= MIN_RANK)
                {
                    ((DOMAINNODE_T *) dat)->updateCount++;
                    urlNode->updateTime = time(NULL);
                    urlNode->sendNum++;
                    break;
                }
            }
            urlNode = NULL;
        }

        if (urlNode != NULL)
        {
            memcpy(&tmpKey, urlKey, sizeof(KEY_T));
            memcpy(&tmpNode, urlNode, sizeof(URLNODE_T));
        }
        pthread_mutex_unlock(&cache_mutex);

        if (tmpKey != 0)
        {
            if (neo_bdb_search(&tmpKey, url) == 0)
            {
                printf("node send -> (%s), urlStat = %d\n", url,
                       tmpNode.urlStat);
                node_send(&tmpNode, url);
            }
        }
    } else if (strcmp((char *) args, "DUMP") == 0)
    {
        dumpfp = fopen(dumpfile, "ab+");
        assert(dumpfp);

        dumpDomainKey = (KEY_T *) key;
        if (!dumpDomainKey || !*dumpDomainKey
            || !((DOMAINNODE_T *) dat)->urlDict)
            return 0;

        rb_dict_walk(((DOMAINNODE_T *) dat)->urlDict, visit, "SAVE");

        fclose(dumpfp);
        dumpfp = NULL;
    } else if (strcmp((char *) args, "SAVE") == 0)
    {
        assert(((URLNODE_T *) dat)->urlLen < MAX_URL_LEN);

        fwrite(dumpDomainKey, sizeof(KEY_T), 1, dumpfp);
        fwrite(key, sizeof(KEY_T), 1, dumpfp);
        fwrite(dat, sizeof(URLNODE_T), 1, dumpfp);
    } else if (strcmp((char *) args, "LOG") == 0)
    {
        fprintf(logfp, "[INS=%d]\t[UPD=%d]\t[TOTAL=%d]\t%s\n",
                ((DOMAINNODE_T *) dat)->insertCount,
                ((DOMAINNODE_T *) dat)->updateCount,
                rb_dict_count(((DOMAINNODE_T *) dat)->urlDict),
                ((DOMAINNODE_T *) dat)->domain);

        total_count += rb_dict_count(((DOMAINNODE_T *) dat)->urlDict);
    }

    return 0;
}

int log_thr()
{
    while (1)
    {
        sleep(300);
        total_count = 0;
        logfp = fopen("stat.log", "w");
        assert(logfp);

        pthread_mutex_lock(&cache_mutex);
        rb_dict_walk(g_domainRBdict, visit, "LOG");
        pthread_mutex_unlock(&cache_mutex);

        fprintf(logfp, "MACHINE SUM = %d\n", total_count);

        fclose(logfp);
    }

    return 0;
}

int dump_thr()
{
    while (1)
    {
        sleep(7200);
        printf("begin to (%d)dump cache to dumpfile\n", ++dumpCount);

        pthread_mutex_lock(&cache_mutex);
        rename(dumpfile, "dumpfile.111");
        rb_dict_walk(g_domainRBdict, visit, "DUMP");

        if (dumpfp != NULL)
        {
            fclose(dumpfp);
            dumpfp = NULL;
        }
        pthread_mutex_unlock(&cache_mutex);

        printf("success to (%d)dump cache to dumpfile\n", dumpCount);
    }

    return 0;
}

int select_update_thr()
{
    while (1)
    {
        rb_dict_walk(g_domainRBdict, visit, "UPDATE");
        usleep(500000);
    }

    return 0;
}

int fetch_url_thr()
{
    char url[MAX_URL_LEN];
    FILE *fetchfp = NULL;
    URLNODE_T urlNode;

    while (1)
    {
        usleep(500000);
        rename("./xx_no_fetch_url", "./xx_fetchin_url");

        fetchfp = fopen("./xx_fetchin_url", "r");
        if (fetchfp == NULL)
        {
            continue;
        }

        while (fgets(url, MAX_URL_LEN, fetchfp) != NULL)
        {
            int len = strlen(url);
            if(len <= 8) continue;

            url[len-1] = '\0';

            bzero(&urlNode, sizeof(URLNODE_T));
            urlNode.urlLen = len-1;
            urlNode.urlType = NORMAL;

            node_send(&urlNode, url);
        }

        fclose(fetchfp);

        unlink("./xx_fetchin_url");
    }

    return 0;
}
