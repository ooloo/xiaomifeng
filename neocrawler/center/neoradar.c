#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "centerCache.h"
#include "centerSelector.h"
#include "centerbdb.h"
#include "list.h"
#include "hashtable.h"

#define MAX_LIST_NUM	640000
#define HASH_SLOT_NUM   5000011

struct _rlist {
    struct list_head rhead;
    URLNODE_T node;
};

int listNum = 0;
int g_basePort = 0;
LIST_HEAD(radarList);
TDict *link_dict = NULL;
pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;

void link_init()
{
    assert(link_dict = hashtable_init(HASH_SLOT_NUM));

    return;
}

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
    int ret;
    unsigned slotNo, key;
    TNodeInfo info;

    assert(dict && keyStr);

    hashtable_hash(dict, &slotNo, &key, keyStr);

    ret = hashtable_seek(dict, slotNo, key, &info);

    return ret;
}

int link_delete(TDict * dict, char *keyStr)
{
    unsigned slotNo, key;

    hashtable_hash(dict, &slotNo, &key, keyStr);

    if (hashtable_delete(dict, slotNo, key) != 1)
        printf("@@@@@@@@@@@@@@@@@@delete error @@@@@@@@@@@@@@@@@@@@@@\n");
}

int radar_list_add(URLNODE_T * node, char *url, struct list_head *head)
{
    void *listBuf;
    struct _rlist *radarNode;

    assert(node->urlLen == strlen(url));

    listBuf = malloc(sizeof(struct _rlist) + node->urlLen + 1);
    assert(listBuf);

    radarNode = (struct _rlist *) listBuf;

    memcpy(&radarNode->node, node, sizeof(URLNODE_T));
    strcpy((char *) (listBuf + sizeof(struct _rlist)), url);

    list_add_tail(&radarNode->rhead, head);
    listNum++;

    return 0;
}

int radar_list_get(URLNODE_T * node, char *url, struct list_head *head)
{
    void *listBuf;
    struct _rlist *radarNode;

    if (list_empty(head))
        return -1;

    radarNode = list_entry(head->next, typeof(struct _rlist), rhead);
    assert(radarNode);

    listBuf = (void *) radarNode;
    memcpy(node, &radarNode->node, sizeof(URLNODE_T));

    assert(strlen(listBuf + sizeof(struct _rlist)) < MAX_URL_LEN);
    strcpy(url, (char *) (listBuf + sizeof(struct _rlist)));

    list_del(head->next);
    listNum--;

    free(listBuf);

    return 0;
}

int radar_send(int sendSock, URLNODE_T * urlNode, char *url)
{
    int len;
    char *p, buf[2048];

    assert(urlNode && url);

    len = sizeof(URLNODE_T) + urlNode->urlLen;
    assert(len < 2048);

    memcpy(buf, urlNode, sizeof(URLNODE_T));
    memcpy(buf + sizeof(URLNODE_T), url, urlNode->urlLen);

    if (htcp_send(sendSock, buf, len) == -1)
    {
        printf("radar_send error, socket=%d\n", sendSock);
        close(sendSock);
        return -1;
    }

    return 0;
}

int radar_recv(int recvSock, void *buf, int len)
{
    assert(buf && len);

    if (htcp_recv(recvSock, buf, len) == -1)
    {
        printf("radar_recv error, socket=%d\n", recvSock);
        close(recvSock);
        return -1;
    }

    return 0;
}

void transformer_storage(URLNODE_T * urlNode, char *url, int socket)
{
    int sinLen;
    char str[2048], key[16];
    struct sockaddr_in sin;

    sinLen = sizeof(struct sockaddr_in);
    getpeername(socket, (struct sockaddr *) &sin, &sinLen);

    snprintf(str, 2048, "%d %d %u %u %u %u %u %u %u %u %u %u %u %s %s",
             urlNode->insertTime,
             urlNode->updateTime,
             urlNode->urlType,
             urlNode->urlStat,
             urlNode->urlRank,
             urlNode->urlOther,
             urlNode->urlLen,
             urlNode->linkNum,
             urlNode->sendNum,
             urlNode->statusCode,
             urlNode->bbsMysqlId,
             urlNode->linkPickInfo,
             urlNode->htmlLen,
             url,
             inet_ntoa(sin.sin_addr));

    assert(MD5(url, strlen(url), (unsigned char *) key));
    neo_bdb_insert(key, str);

    return;
}

void work_recv_thr(int socket)
{
    char *url, buf[2048], dat[2048];
    unsigned char key[16];
    URLNODE_T *urlNode;

    urlNode = (URLNODE_T *) buf;
    url = buf + sizeof(URLNODE_T);

    pthread_detach(pthread_self());

    while (1)
    {
        if (radar_recv(socket, buf, 2048) == -1)
            return;

        if (urlNode->urlLen >= MAX_URL_LEN - 1)
        {
            printf("urlNode->urlLen error.\n");
            continue;
        }

        *(url + urlNode->urlLen) = 0;
        if (urlNode->insertTime == 0)
            urlNode->insertTime = time(NULL);

        printf("url: %s\n", url);
        // rrrrrrrrrrrrrrrrrrre do
        //transformer_storage(urlNode, url, socket);
        //continue;
        // rrrrrrrrrrrrrrrrrrre do

        if (urlNode->updateTime > 0)
        {
            if (urlNode->urlStat != UNCHANGE)
                transformer_storage(urlNode, url, socket);
        } else if (listNum <= MAX_LIST_NUM)
        {
            // tmp cache
            pthread_mutex_lock(&list_mutex);
            if (link_search(link_dict, url) == 0)
            {
                // not found
                if (link_dict->nodeNum < 5 * NODE_NUM_PER_BLK - 1024)
                {
                    link_insert(link_dict, url);
                } else
                {
                    hashtable_free(link_dict);
                    link_init();
                }
            } else
            {
                pthread_mutex_unlock(&list_mutex);
                continue;
            }
            pthread_mutex_unlock(&list_mutex);

            assert(MD5(url, strlen(url), (unsigned char *) key));

            // not found in bdb
            if (neo_bdb_search(key, dat) == -1)
            {
                pthread_mutex_lock(&list_mutex);
                radar_list_add(urlNode, url, &radarList);
                pthread_mutex_unlock(&list_mutex);
            }
        }
    }

    return;
}

void work_send_thr(int socket)
{
    int ret;
    char *url, buf[2048];
    URLNODE_T *urlNode;

    urlNode = (URLNODE_T *) buf;
    url = buf + sizeof(URLNODE_T);

    pthread_detach(pthread_self());

    while (1)
    {
        pthread_mutex_lock(&list_mutex);
        ret = radar_list_get(urlNode, url, &radarList);
        pthread_mutex_unlock(&list_mutex);

        if (ret == -1)
        {
            sleep(1);
            continue;
        }

        if (radar_send(socket, urlNode, url) == -1)
            return;

        printf("radar send: (%d) %s\n", listNum, url);

        transformer_storage(urlNode, url, socket);

        //pthread_mutex_lock(&list_mutex);
        //link_delete(link_dict, url);
        //pthread_mutex_unlock(&list_mutex);
    }

    return;
}

void accept_recv_thr()
{
    int ret, listenSock, isocket;
    pthread_t acceptid;

    listenSock = htcp_listen(g_basePort + 47, 5);
    assert(listenSock);
    printf("htcp_listen %d success.\n", g_basePort + 47);

    while (1)
    {
        isocket = htcp_accept(listenSock, NULL, NULL);
        assert(isocket);

        ret =
            pthread_create(&acceptid, NULL, (void *) work_recv_thr,
                           (void *) isocket);
        if (ret != 0)
        {
            printf("create work_recv_thr derror.\n");
            exit(1);
        }
        sleep(1);
    }

    pthread_join(acceptid, NULL);

    return;
}

void accept_send_thr()
{
    int ret, listenSock, isocket;
    pthread_t acceptid;

    listenSock = htcp_listen(g_basePort + 46, 5);
    assert(listenSock);
    printf("htcp_listen %d success.\n", g_basePort + 46);

    while (1)
    {
        isocket = htcp_accept(listenSock, NULL, NULL);
        assert(isocket);

        ret =
            pthread_create(&acceptid, NULL, (void *) work_send_thr,
                           (void *) isocket);
        if (ret != 0)
        {
            printf("create work_send_thr error.\n");
            exit(1);
        }
        sleep(1);
    }

    pthread_join(acceptid, NULL);

    return;
}

int main(int argc, char **argv)
{
    int ret;
    pthread_t recvid, sendid;

    if (argc != 2)
    {
        printf("%s baseport\n", argv[0]);
        exit(1);
    }

    g_basePort = atoi(argv[1]);
    assert(g_basePort > 1024 && g_basePort < 65535);

    link_init();
    neo_bdb_init(DB_CREATE);
    printf("bdb open success.\n");

    ret = pthread_create(&recvid, NULL, (void *) accept_recv_thr, NULL);
    if (ret != 0)
    {
        printf("create work_thr thr error.\n");
        exit(1);
    }

    ret = pthread_create(&sendid, NULL, (void *) accept_send_thr, NULL);
    if (ret != 0)
    {
        printf("create work_thr thr error.\n");
        exit(1);
    }

    pthread_join(recvid, NULL);
    pthread_join(sendid, NULL);

    neo_bdb_close();

    return 0;
}
