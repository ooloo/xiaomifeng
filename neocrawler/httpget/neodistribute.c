#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include "hashtable.h"


#define MAX_HTML_LEN (512*1024)
#define HASH_SLOT_NUM 5000011

int g_basePort = 0;
TDict *link_dict = NULL;

pthread_mutex_t recv_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t hash_mutex = PTHREAD_MUTEX_INITIALIZER;


void link_insert(TDict * dict, char *keyStr)
{
    unsigned slotNo, key;
    TNodeInfo info;

    assert(dict && keyStr);

    hashtable_hash(dict, &slotNo, &key, keyStr);

    assert(hashtable_insert(dict, slotNo, key, &info) != -1);

    return;
}

// return 1:found 0:notfound
int link_search(TDict * dict, char *keyStr)
{
    unsigned slotNo, key;
    TNodeInfo info;

    assert(dict && keyStr);

    hashtable_hash(dict, &slotNo, &key, keyStr);

    return hashtable_seek(dict, slotNo, key, &info);
}

void link_init(char *filename)
{
    FILE *fp;
    char *t, src[MAX_URL_LEN], path[MAX_URL_LEN];

    assert(link_dict = hashtable_init(HASH_SLOT_NUM));

    fp = fopen(filename, "r");
    assert(fp);

    while (fgets(src, MAX_URL_LEN, fp))
    {
        sscanf(src, "%s", path);
        if (strncasecmp(path, "http://", 7))
        {
            snprintf(src, MAX_URL_LEN, "http://%s", path);
            strcpy(path, src);
        }
        if (strchr(path, '/') == NULL)
        {
            strcat(path, "/");
        }
        t = path;
        while (*t)
        {
            if (isupper(*t))
                *t += 32;
            t++;
        }
        t = strrchr(path, '/');
        assert(t);

        *(t + 1) = 0;

        link_insert(link_dict, path);
    }

    fclose(fp);
}

// return 0: need filter
int link_filter(char *url)
{
    int depth = 0;
    char *p = NULL, *pp = NULL;
    char path[MAX_URL_LEN];

    p = url + 7;
    while (*p)
    {
        if (*p == '/')
            depth++;
        p++;
    }

    if (depth > 3 || strlen(url) > 128)
        return 0;

    if (p = strchr(url, '?'))
    {
        p++;
        if (strchr(p, '?') || strlen(url) > 64)
        {
            return 0;
        }
    } else
    {
        if (!strstr(url, "htm"))
        {
            return 0;
        }
    }

    pp = url + 7;
    while (p = strchr(pp, '/'))
    {
        p++;
        assert(p - url < MAX_URL_LEN);
        strncpy(path, url, p - url);
        *(path + (p - url)) = 0;

        if (link_search(link_dict, path) == 1)
            return 1;

        pp = p;
    }

    return 0;
}

int node_recv(int recvSock, void *buf, int len)
{
    assert(buf && len && len > sizeof(URLNODE_T));

    if (htcp_recv(recvSock, buf, len) == -1)
    {
        close(recvSock);
        return -1;
    }

    return 0;
}

int node_send(URLNODE_T * urlNode, char *url)
{
    int len;
    char *p, buf[2048];

    assert(urlNode && url);

    urlNode->urlLen = strlen(url);

    len = sizeof(URLNODE_T) + urlNode->urlLen;
    assert(len < 2048);

    memcpy(buf, urlNode, sizeof(URLNODE_T));
    memcpy(buf + sizeof(URLNODE_T), url, urlNode->urlLen);

    pthread_mutex_lock(&send_mutex);
    //printf("sending to center.............\n");
    while (htcp_send(sendSock, buf, len) == -1)
    {
        sleep(1);
        close(sendSock);

        while ((sendSock =
                htcp_connect("127.0.0.1", g_basePort + 1)) == -1)
        {
            printf("htcp_connect error.\n");
            sleep(1);
        }
        printf("send htcp_connect success.\n");
    }
    pthread_mutex_unlock(&send_mutex);

    return 0;
}

int distribute_thr(int recvSock)
{
    char *p, buf[2048], *url;
    URLNODE_T *urlNode;

    urlNode = (URLNODE_T *) buf;
    url = buf + sizeof(URLNODE_T);

    while (1)
    {
        node_recv(recvSock, buf, 2048);
        assert(urlNode->urlLen + sizeof(URLNODE_T) < 2048);

        *(url + urlNode->urlLen) = 0;
        printf("recv: %s urlLen: %d", url, urlNode->urlLen);

        if (link_filter(url) == 0)
            continue;

        node_send(urlNode, url);
        printf("send (%s) to center\n", links->links[i]);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int i, ret;
    pthread_t id;

    if (argc != 3)
        exit(1);

    mylinkExtract_init();

    link_init(argv[2]);

    g_basePort = atoi(argv[1]);
    assert(g_basePort > 1024 && g_basePort < 65535);

    for (i = 0; i < 8; i++)
    {
        ret = pthread_create(&id, NULL, (void *) html_produce, NULL);
        if (ret != 0)
        {
            printf("create thr error.\n");
            exit(1);
        }
    }

    pthread_join(id, NULL);

    return 0;
}
