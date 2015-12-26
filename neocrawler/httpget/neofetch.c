#define _GNU_SOURCE

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "http_fetcher.h"
#include "centerCache.h"

int recvSock = -1;
int sendSock = -1;
int sendListenSock = -1;
int g_basePort = 0;
int g_fetchNum = 0;
pthread_mutex_t recv_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;

int node_recv(void *buf, int len)
{
    assert(buf && len);

    pthread_mutex_lock(&recv_mutex);

    while (htcp_recv(recvSock, buf, len) == -1)
    {
        sleep(1);
        close(recvSock);

        while ((recvSock = htcp_connect("127.0.0.1", g_basePort)) == -1)
        {
            sleep(1);
            printf("htcp_connect error.\n");
        }
        printf("htcp_connect success.\n");
    }

    pthread_mutex_unlock(&recv_mutex);

    return 0;
}

int node_send(char *buf, char *html)
{
    int len = 0;
    URLNODE_T *node;

    node = (URLNODE_T *) buf;

    node->htmlLen =
        node->htmlLen <=
        (MAX_HTML_LEN - 1) ? node->htmlLen : (MAX_HTML_LEN - 1);

    pthread_mutex_lock(&send_mutex);
    while (htcp_send(sendSock, buf, sizeof(URLNODE_T) + node->urlLen) == -1
           || htcp_send(sendSock, html, node->htmlLen + 1) == -1)
    {
        sleep(1);
        close(sendSock);

        if (sendListenSock == -1)
        {
            sendListenSock = htcp_listen(g_basePort + 2, 5);
            assert(sendListenSock);
        }
        printf("send htcp_listen success [%d].\n", g_basePort + 2);

        while ((sendSock = htcp_accept(sendListenSock, NULL, NULL)) == -1)
        {
            printf("accept error.\n");
            sleep(1);
        }
        printf("send htcp_accept success.\n");
    }
    pthread_mutex_unlock(&send_mutex);

    return 0;
}

int http_get()
{
    int ret;
    char *url = NULL, *fileBuf = NULL;
    char buf[2048];
    URLNODE_T *node;

    node = (URLNODE_T *) buf;
    url = buf + sizeof(URLNODE_T);

    while (1)
    {
        node_recv(buf, 2048);
        printf("node_recv OK.\n");

        assert(node->urlLen < 1024);
        *(url + node->urlLen) = 0;
        node->statusCode = 0;

        ret = http_fetch(url, &fileBuf, &node->statusCode);     /* Downloads page */
        if (ret <= 0)           /* All HTTP Fetcher functions return */
        {
            node->htmlLen = 0;
            printf("(%s)--->error:[%d], statusCode: %d\n", url, http_errno,
                   node->statusCode);

            node_send(buf, "");
            printf("(%s) node_send OK.\n", url);
        } else
        {
            if (node->htmlLen == ret)
                node->urlStat = UNCHANGE;
            else
                node->htmlLen = ret;
            printf("(%d)Page successfully downloaded(%d). (%s)\n",
                   g_fetchNum++, ret, url);

            node_send(buf, fileBuf);
            printf("(%s) node_send OK.\n", url);
        }

        if (fileBuf != NULL)
        {
            free(fileBuf);
            fileBuf = NULL;
        }
    }

    return;
}

int main(int argc, char **argv)
{
    int i, ret;
    pthread_t id;
    pthread_attr_t attr;

    if (argc != 2)
    {
        printf("%s baseport\n", argv[0]);
        exit(1);
    }

    g_basePort = atoi(argv[1]);
    assert(g_basePort > 1024 && g_basePort < 65535);

    assert(pthread_attr_init(&attr) == 0);
    assert(pthread_attr_setstacksize(&attr, 1048576) == 0);

    for (i = 0; i < 8; i++)
    {
        ret = pthread_create(&id, &attr, (void *) http_get, NULL);
        if (ret != 0)
        {
            printf("create thr error.\n");
            exit(1);
        }
    }

    pthread_join(id, NULL);

    return 0;
}
