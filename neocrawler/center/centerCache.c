#include <assert.h>
#include <stdio.h>
#include <netdb.h>
#include <openssl/md5.h>
#include "rbdict.h"
#include "centerCache.h"

#define MAX_BLK_NUM 640000

int g_insertNum = 0;
void *g_cacheStart = NULL;
TRbDict *g_domainRBdict = NULL;
pthread_mutex_t cache_mutex = PTHREAD_MUTEX_INITIALIZER;

char dumpfile[] = "dumpfile.000";

void *use_memory()
{
    int len;
    void *tmp;

    len = sizeof(KEY_T) + sizeof(URLNODE_T);
    if (g_cacheStart == NULL || g_insertNum >= MAX_BLK_NUM)
    {
        g_cacheStart = calloc(len, MAX_BLK_NUM);
        assert(g_cacheStart);

        g_insertNum = 0;
    }

    tmp = g_cacheStart + len * g_insertNum;
    g_insertNum++;

    return tmp;
}

int get_domain(char *url, char *domain, int maxLen)
{
    int dotFlag = 0;
    char *s = NULL, *t = NULL;

    assert(url && domain);

    if (strncasecmp(url, "http://", 7))
        s = t = url;
    else
        s = t = url + 7;

    while (*t && !isspace(*t))
    {
        if (*t == ':' || *t == '/')
            break;

        if (isupper(*t))
            *t += 32;

        if (*t == '.')
            dotFlag = 1;

        t++;
    }

    if (t - s >= maxLen || t - s <= 3 || dotFlag == 0)
        return -1;
    else
        strncpy(domain, s, t - s);

    *(domain + (t - s)) = 0;

    return 0;
}

char *url_trim(char *url)
{
    if (strlen(url) < MAX_URL_LEN - 2 && !strchr(url + 8, '/'))
        strcat(url, "/");

    if (strcasestr(url, "://"))
        return url;
    else
        return NULL;
}

int key_cmp(const void *key1, const void *key2)
{
    if (*(KEY_T *) key1 > *(KEY_T *) key2)
        return 1;
    else if (*(KEY_T *) key1 == *(KEY_T *) key2)
        return 0;
    else
        return -1;
}

int NEO_cache_init()
{
    g_domainRBdict = rb_dict_new((void *) key_cmp, NULL, NULL);
    assert(g_domainRBdict);

    return 0;
}

int __cache_init(DOMAINNODE_T * node)
{
    node->urlDict = rb_dict_new((void *) key_cmp, NULL, NULL);
    assert(node->urlDict);

    node->updateItor = rb_itor_new(node->urlDict);
    assert(node->updateItor);

    return 0;
}

URLNODE_T *__cache_url(TRbDict * dict, KEY_T * domainKey, char *str,
                       URLNODE_T * node)
{
    int nodeLen, strLen;
    unsigned signs[4];
    void *tmp = NULL;
    FILE *dumpfp = NULL;
    URLNODE_T *tmpNode;

    str = url_trim(str);
    if (str == NULL)
        return NULL;

    strLen = strlen(str);

    nodeLen = sizeof(URLNODE_T);

    assert(MD5(str, strLen, (unsigned char *) signs));

    // found!!!
    if (rb_dict_search(dict, signs, &tmp) == 0)
    {
        if (tmp != NULL && node->urlStat == UPDATE)
        {
            // update urlNode, can not memcpy!!!
            // printf("update --> (%s)\n", str);
            assert(((URLNODE_T *) tmp)->urlLen == strLen);
            memcpy(tmp, node, sizeof(URLNODE_T));
        }
        return NULL;
    }
    // not found!!! insert
    // printf("(%d)insert --> (%s)\n", g_insertNum++, str);
    tmp = (void *) use_memory();
    assert(tmp);

    memcpy(tmp, signs, sizeof(KEY_T));

    // new node stat!!!
    tmpNode = (URLNODE_T *) (tmp + sizeof(KEY_T));
    memcpy(tmpNode, node, sizeof(URLNODE_T));

    if (tmpNode->insertTime == 0)
        tmpNode->insertTime = time(NULL);
    tmpNode->urlLen = strLen;

    neo_bdb_insert(tmp, str);

    rb_dict_insert(dict, tmp, tmp + sizeof(KEY_T), 0);

    // record it
    if (domainKey != NULL)
    {
        dumpfp = fopen(dumpfile, "ab+");
        assert(dumpfp);

        assert(fwrite(domainKey, sizeof(KEY_T), 1, dumpfp) == 1);
        assert(fwrite(tmp, sizeof(KEY_T), 1, dumpfp) == 1);
        assert(fwrite(tmpNode, sizeof(URLNODE_T), 1, dumpfp) == 1);

        fclose(dumpfp);
    }

    return NULL;
}

int NEO_cache_domain(TRbDict * dict, char *str, URLNODE_T * urlNode)
{
    int nodeLen;
    unsigned signs[4];
    char domain[MAX_URL_LEN];
    void *tmp;
    DOMAINNODE_T *domainNode;

    if (get_domain(str, domain, MAX_URL_LEN) == -1)
        return -1;

    assert(MD5(domain, strlen(domain), (unsigned char *) signs));

    if (rb_dict_search(dict, signs, (void **) &domainNode) == 0)
    {
        // found!!!
        assert(domainNode && domainNode->urlDict);
        printf("src : %llu %s\n", *(KEY_T *) signs, domain);
    } else
    {
        // not found!!!
        tmp = (void *) calloc(sizeof(KEY_T), +sizeof(DOMAINNODE_T));
        assert(tmp);

        memcpy(tmp, signs, sizeof(KEY_T));

        // insert a new domain 
        rb_dict_insert(dict, tmp, tmp + sizeof(KEY_T), 0);

        // creat urlcache about this domain
        domainNode = (DOMAINNODE_T *) (tmp + sizeof(KEY_T));
        snprintf(domainNode->domain, MAX_DOMAIN_LEN, "%s", domain);

        __cache_init(domainNode);
    }

    __cache_url(domainNode->urlDict, NULL, str, urlNode);

    return 0;
}

int NEO_cache_load(TRbDict * dict, KEY_T * domainKey, KEY_T * urlKey,
                   URLNODE_T * node)
{
    int ret;
    void *tmp = NULL;
    char str[2048];
    URLNODE_T *urlNode;
    DOMAINNODE_T *domainNode = NULL;

    if (rb_dict_search(dict, domainKey, (void **) &domainNode) != 0)
    {
        printf("found extern domain: %llu\n", *domainKey);
        if (neo_bdb_search(urlKey, str) == 0)
            ret = NEO_cache_domain(g_domainRBdict, str, node);
        else
            return 0;

        if (rb_dict_search(dict, domainKey, (void **) &domainNode) != 0)
            return 0;
    }

    assert(domainNode && domainNode->urlDict);

    if (rb_dict_search(domainNode->urlDict, urlKey, (void **) &urlNode) ==
        0)
    {
        if (urlNode != NULL)
        {
            if (urlNode->urlType == MANUAL)
                node->urlType = MANUAL;
            memcpy(urlNode, node, sizeof(URLNODE_T));
        }
        return 0;
    }

    tmp = (void *) use_memory();
    assert(tmp);

    memcpy(tmp, urlKey, sizeof(KEY_T));
    urlNode = (URLNODE_T *) (tmp + sizeof(KEY_T));

    memcpy(urlNode, node, sizeof(URLNODE_T));

    rb_dict_insert(domainNode->urlDict, tmp, tmp + sizeof(KEY_T), 1);

    return 0;
}

int NEO_cache_url(TRbDict * dict, char *str, URLNODE_T * node)
{
    unsigned signs[4], urlid[4];
    char domain[MAX_URL_LEN];
    struct hostent *he = NULL;
    FILE *dumpfp = NULL;
    DOMAINNODE_T *tmp = NULL;

    //-------------- rank ------------------
    if (node->urlStat == UPDATE && node->htmlLen > 0)
        node->urlOther++;
    else
        node->urlOther = 0;

    if (node->urlRank == 0)
    {
        if (node->urlType == MANUAL)
            node->urlRank = MAX_RANK - 24;
        else if (node->urlType == INDEX)
            node->urlRank = MAX_RANK - 48;
        else
            node->urlRank = MIN_RANK * 4;
    } else
    {
        if (node->urlOther >= 3 && node->urlRank < MAX_RANK - 16)
            node->urlRank++;
        else if (node->urlStat == UNCHANGE)
            node->urlRank--;
        else if (node->htmlLen <= 0)
            node->urlRank /= 2;
    }
    //-------------- rank ------------------

    if (get_domain(str, domain, MAX_URL_LEN) == -1)
        return -1;

    assert(MD5(domain, strlen(domain), (unsigned char *) signs));
    assert(MD5(str, strlen(str), (unsigned char *) urlid));

    if (rb_dict_search(dict, signs, (void **) &tmp) != 0)
    {
        //#ifndef BBS
        //              he = gethostbyname(domain);
        //              if(he == NULL)
        //                      return -1;
        //#endif

        // insert domain & url to cahae
        printf("warning new domain: %s\n", str);
        NEO_cache_domain(g_domainRBdict, str, node);

        // save to dumpfile
        dumpfp = fopen(dumpfile, "ab+");
        assert(dumpfp);

        assert(fwrite(signs, sizeof(KEY_T), 1, dumpfp) == 1);
        assert(fwrite(urlid, sizeof(KEY_T), 1, dumpfp) == 1);
        assert(fwrite(node, sizeof(URLNODE_T), 1, dumpfp) == 1);

        fclose(dumpfp);
    } else
    {
        assert(tmp && tmp->urlDict);
        __cache_url(tmp->urlDict, (KEY_T *) signs, str, node);
    }

    return 0;
}

unsigned NEO_cache_check(TRbDict * dict, char *str)
{
    unsigned signs[4];
    char domain[MAX_URL_LEN];
    DOMAINNODE_T *tmp = NULL;

    if (get_domain(str, domain, MAX_URL_LEN) == -1)
        return (unsigned) -1;

    assert(MD5(domain, strlen(domain), (unsigned char *) signs));

    if (rb_dict_search(dict, signs, (void **) &tmp) != 0)
    {
        return 0;
    } else
    {
        assert(tmp);
        return rb_dict_count(((DOMAINNODE_T *) tmp)->urlDict);
    }

    return 0;
}
