#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <math.h>
#include "mysql.h"
#include "neomysql.h"

MYSQL *SDNSmysql = NULL;        //mysql
char *SDNSquery = NULL;         //mysql query string


/*-----------------------------------------------
 *	addr数据库清空
 *
 *	即将addr内数据全部删除
 *---------------------------------------------*/

void sdns_mysql_ipnote_init(MYSQL * mysql)
{
    int queryLen, selectNum = 0;
    char query[512];
    MYSQL_RES *mysqlRes = NULL; //查询结果存储

    assert(mysql);

    sprintf(query, "select * from ipnote");
    queryLen = strlen(query);

    mysql_real_query(mysql, query, queryLen);

    mysqlRes = mysql_store_result(mysql);
    if (!mysqlRes)
    {
        slog_write(LL_TRACE, "ipnote_init Mysql Failed %d: %s\n",
                   mysql_errno(mysql), mysql_error(mysql));
        exit(1);
    }

    selectNum = mysql_num_rows(mysqlRes);

    mysql_free_result(mysqlRes);

    if (selectNum != 0)
        return;

    //清空ipnote表
    sprintf(query, "delete from ipnote");
    queryLen = strlen(query);

    mysql_real_query(mysql, query, queryLen);

    //重建ipnote
    sdns_mysql_ipnote_create(mysql);

    return;
}


/*----------------------------------------
 * if domain is a digit domain,
 * 
 * chech whether it exist or not
 * 
 * return:
 * 			-1 : error
 * 		     0 : useful digit domain
 *---------------------------------------*/

int sdns_mysql_ipnote_check(MYSQL * mysql, char *ipdomain)
{
    int queryLen, selectNum;
    char query[512];
    MYSQL_RES *mysqlRes = NULL; //查询结果存储

    //参数错误
    if (!mysql || !*ipdomain || !ipdomain)
        return -1;

    //查询数据库
    sprintf(query, "select * from ipnote where ip='%s'", ipdomain);
    queryLen = strlen(query);

    if (mysql_real_query(mysql, query, queryLen))
    {
        slog_write(LL_TRACE, "ipnote_store Mysql query: %s\n", query);
        slog_write(LL_TRACE, "ipnote_query Mysql Failed %d: %s\n",
                   mysql_errno(mysql), mysql_error(mysql));
        exit(1);
    }

    mysqlRes = mysql_store_result(mysql);
    if (!mysqlRes)
    {
        slog_write(LL_TRACE, "ipnote_store Mysql Failed %d: %s\n",
                   mysql_errno(mysql), mysql_error(mysql));
        exit(1);
    }
    //查询结果
    selectNum = mysql_num_rows(mysqlRes);

    mysql_free_result(mysqlRes);

    if (selectNum == 0)
    {
        return 0;
    } else
    {
        return -1;
    }
}


/*-----------------------------------------------
 * 	发现一个ip地址对应过多domain的情况
 * 	return:
 * 		-1	error
 *	   	>0	success
 *	   	 0	spam
 *---------------------------------------------*/

int sdns_mysql_ipnote_query(MYSQL * mysql, char *ipdomain, char *domain)
{
    int queryLen, domainNum = 0, selectNum = 0;
    char query[512];
    MYSQL_RES *mysqlRes = NULL; //查询结果存储
    MYSQL_ROW mysqlRow;

    //参数错误
    if (!mysql || !*ipdomain || !ipdomain)
        return -1;

    //查询数据库
    sprintf(query, "select * from dnscache where domain='%s'", domain);
    queryLen = strlen(query);

    if (mysql_real_query(mysql, query, queryLen))
    {
        slog_write(LL_TRACE, "dnscache_query Mysql query: %s\n", query);
        slog_write(LL_TRACE, "dnscache_query Mysql Failed %d: %s\n",
                   mysql_errno(mysql), mysql_error(mysql));
        exit(1);
    }

    mysqlRes = mysql_store_result(mysql);
    if (!mysqlRes)
    {
        slog_write(LL_TRACE, "dnscache_query Mysql Failed %d: %s\n",
                   mysql_errno(mysql), mysql_error(mysql));
        exit(1);
    }

    selectNum = mysql_num_rows(mysqlRes);

    mysql_free_result(mysqlRes);
    mysqlRes = NULL;

    // found this domain, do nothing
    if (selectNum != 0)
    {
        return 1;
    }
    //查询数据库
    sprintf(query, "select * from ipnote where ip='%s'", ipdomain);
    queryLen = strlen(query);

    if (mysql_real_query(mysql, query, queryLen))
    {
        slog_write(LL_TRACE, "ipnote_query Mysql query: %s\n", query);
        slog_write(LL_TRACE, "ipnote_query Mysql Failed %d: %s\n",
                   mysql_errno(mysql), mysql_error(mysql));
        exit(1);
    }

    mysqlRes = mysql_store_result(mysql);
    if (!mysqlRes)
    {
        slog_write(LL_TRACE, "ipnote_query Mysql Failed %d: %s\n",
                   mysql_errno(mysql), mysql_error(mysql));
        exit(1);
    }
    //查询结果
    selectNum = mysql_num_rows(mysqlRes);

    if (selectNum == 0)
    {
        sprintf(query, "insert into ipnote(ip,domainNum) values('%s',%d)",
                ipdomain, 1);
        queryLen = strlen(query);

        if (mysql_real_query(mysql, query, queryLen))
        {
            slog_write(LL_TRACE, "1 Mysql Failed %d: %s\n",
                       mysql_errno(mysql), mysql_error(mysql));
            exit(1);
        }
    } else
    {
        mysqlRow = mysql_fetch_row(mysqlRes);
        domainNum = atoi(mysqlRow[1]);

        if (domainNum >= MAX_DOMAIN_NUM_PER_IP &&
            ((double) domainNum) * rand() / (RAND_MAX + 1.0) >
            sqrt(1.0 * MAX_DOMAIN_NUM_PER_IP) * sqrt(1.0 * domainNum))
        {
            //if(domain != NULL)    // && *domain!=0
            //{
            //      sprintf(query,"delete from dnscache where domain='%s'",domain);
            //      queryLen=strlen(query);
            //      
            //      if(mysql_real_query(mysql,query,queryLen))
            //      {
            //              slog_write(LL_TRACE ,"Mysql Failed %d: %s\n",mysql_errno(mysql),query);
            //              exit(1);
            //      }
            //}

            mysql_free_result(mysqlRes);
            return 0;
        } else
        {
            domainNum++;

            sprintf(query, "update ipnote set domainNum=%d where ip='%s'",
                    domainNum, ipdomain);
            queryLen = strlen(query);

            if (mysql_real_query(mysql, query, queryLen))
            {
                slog_write(LL_TRACE, "2 Mysql Failed %d: %s\n",
                           mysql_errno(mysql), mysql_error(mysql));
                exit(1);
            }
        }
    }

    mysql_free_result(mysqlRes);
    return 1;
}


/*-----------------------------------------------
 *	将新解析好的域名、ip、模式存入mysql
 *	lnode	解析信息
 *	mode	0-正解	1-反解
 *	ifdel	删除标志
 *	query	请求字符串
 *	flag	ipnote查询标志
 *
 *	return:
 *		-1	error
 *		 0	success
 *----------------------------------------------*/

int sdns_mysql_insert(MYSQL * mysql, NODE * lnode, int mode, int ifdel)
{
    int queryLen, selectNum;
    char query[512];
    struct in_addr sin;

    MYSQL_RES *mysqlRes = NULL; //查询结果存储

    //参数错误
    if (!mysql || !lnode || !query)
        return -1;

    if (ifdel == 1)
    {
        sprintf(query, "delete from dnscache where domain='%s'",
                lnode->domain);
        queryLen = strlen(query);

        if (mysql_real_query(mysql, query, queryLen))
        {
            slog_write(LL_TRACE, "Delete domain errorNo %d: %s\n",
                       mysql_errno(mysql), mysql_error(mysql));
            exit(1);
        }

        return 0;
    }
    //查询数据库
    sprintf(query, "select * from dnscache where domain='%s'",
            lnode->domain);
    queryLen = strlen(query);

    if (mysql_real_query(mysql, query, queryLen))
    {
        slog_write(LL_TRACE, "3 Mysql query: %s\n", query);
        slog_write(LL_TRACE, "3 Mysql Failed %d: %s\n", mysql_errno(mysql),
                   mysql_error(mysql));
        exit(1);
    }

    mysqlRes = mysql_store_result(mysql);
    if (!mysqlRes)
    {
        slog_write(LL_TRACE, "3 Mysql Failed %d: %s\n", mysql_errno(mysql),
                   mysql_error(mysql));
        exit(1);
    }
    //查询结果
    selectNum = mysql_num_rows(mysqlRes);

    mysql_free_result(mysqlRes);

    sin.s_addr = lnode->addr;

    if (selectNum == 0)
    {
        //查询结果为0，insert
        sprintf(query,
                "insert into dnscache(domain,ip,mode,lastusetime,updatetime,ifdel) "
                "values('%s','%s',%d,%d,%d,%d)", lnode->domain,
                (char *) inet_ntoa(sin), mode, lnode->lastUseTime,
                lnode->updateTime, ifdel);

        queryLen = strlen(query);

        slog_write(LL_TRACE, "INSERT DNSCACHE: %s", query);

        if (mysql_real_query(mysql, query, queryLen))
        {
            slog_write(LL_TRACE, "Insert mysql errorNo %d: %s\n",
                       mysql_errno(mysql), mysql_error(mysql));
            exit(1);
        }
    } else
    {
        //查询结果不为零，update
        sprintf(query,
                "update dnscache set ip='%s',lastusetime=%d,updatetime=%d where domain='%s'",
                (char *) inet_ntoa(sin), lnode->lastUseTime,
                lnode->updateTime, lnode->domain);

        queryLen = strlen(query);

        if (mysql_real_query(mysql, query, queryLen))
        {
            slog_write(LL_TRACE, "Update mysql query Failed.");
            exit(1);
        }

        return 1;
    }

    return 0;
}


/*-------------------------------------
 * 	生成ipnote数据表
 * 	用于发现垃圾domain
 *------------------------------------*/

int sdns_mysql_ipnote_create(MYSQL * mysql)
{
    int i, sum, num = 0;
    struct in_addr sin;
    NODE *tmpnode;

    slog_write(LL_NOTICE,
               "*********** do create ipnote table again ********");

    i = 0;
    sum = 0;
    tmpnode = NULL;
    while (*(chainpointer + i) != NULL)
    {
        sum = 0;
        tmpnode = *(chainpointer + i);
        while (sum < HASH_LEN)
        {
            if ((tmpnode + sum)->domain[0] == '\0')
                break;

            sin.s_addr = (tmpnode + sum)->addr;
            sdns_mysql_ipnote_query(mysql, (char *) inet_ntoa(sin),
                                    (tmpnode + sum)->domain);

            num++;
            sum++;
        }

        if (++i >= MAX_ADD_NUM)
            break;
    }
    slog_write(LL_NOTICE, "total domain num = %d", num);

    return 0;
}


/*---------------------------------------------
 *	写robot信息到MYSQL
 *
 *	domain	域名
 *	BlkNum	robot块数目
 *	robotBlk	robot起始指针
 *	query	请求指令字符串
 *
 *	return:
 *		-1 	error
 *		 0	插入成功
 *		 1	更新成功
 *--------------------------------------------*/


int sdns_robot_write(MYSQL * mysql, char *domain, int BlkNum,
                     ROBOT * robotBlk, char *query)
{
    int queryLen, robotLen, selectNum;
    MYSQL_RES *mysqlRes = NULL;
    struct timeval tv;

    //参数错误
    if (!mysql || !*domain || !domain || !BlkNum || !robotBlk || !query)
        return -1;

    gettimeofday(&tv, NULL);

    //查询somain是否已经存在
    sprintf(query, "select * from robot where domainip='%s'", domain);
    queryLen = strlen(query);

    if (mysql_real_query(mysql, query, queryLen))
    {
        slog_write(LL_TRACE, "4 Mysql query: %s\n", query);
        exit(1);
    }

    mysqlRes = mysql_store_result(mysql);
    if (!mysqlRes)
    {
        slog_write(LL_TRACE, "4 Mysql Failed %d: %s\n", mysql_errno(mysql),
                   mysql_error(mysql));
        exit(1);
    }
    //selectNum为零，不存在，否则存在
    selectNum = mysql_num_rows(mysqlRes);

    mysql_free_result(mysqlRes);

    if (selectNum == 0)
    {
        //不存在，插入
        sprintf(query, "insert into robot values('%s',%d,%d,'", domain,
                BlkNum, (int) tv.tv_sec);
        queryLen = strlen(query);

        robotLen =
            mysql_real_escape_string(mysql, query + queryLen,
                                     (char *) robotBlk,
                                     BlkNum * ROBOT_BLK_SIZE);
        queryLen += robotLen;
        query[queryLen++] = '\'';
        query[queryLen++] = ')';
        query[queryLen] = 0;

        if (mysql_real_query(mysql, query, queryLen))
        {
            slog_write(LL_TRACE, "robot insert Failed %d: %s\n",
                       mysql_errno(mysql), mysql_error(mysql));
            exit(1);
        }
    } else
    {
        //存在，更新
        sprintf(query, "update robot set blksize=%d,updatetime=%d,robot='",
                BlkNum, (int) tv.tv_sec);
        queryLen = strlen(query);

        robotLen =
            mysql_real_escape_string(mysql, query + queryLen,
                                     (char *) robotBlk,
                                     BlkNum * ROBOT_BLK_SIZE);
        queryLen += robotLen;
        query[queryLen++] = '\'';
        query[queryLen++] = ' ';
        sprintf(query + queryLen, "where domainip='%s'", domain);
        queryLen += strlen(query + queryLen);

        if (mysql_real_query(mysql, query, queryLen))
        {
            slog_write(LL_TRACE, "robot update Failed %d: %s\n",
                       mysql_errno(mysql), mysql_error(mysql));
            exit(1);
        }

        return 1;
    }

    return 0;
}


/*--------------------------------------------
 * load mysql robot into memory
 *
 * 启动时，将robot载入内存
 *-------------------------------------------*/

int sdns_robot_load(MYSQL * mysql)
{
    int i = 0, ret, hashkey, queryLen, updatetime;
    MYSQL_RES *mysqlRes = NULL;
    MYSQL_ROW mysqlRow;
    struct timeval tv;
    NODE lnode;

    queryLen = strlen("select * from robot");
    mysql_real_query(mysql, "select * from robot", queryLen);

    // 导入 SDNS cache 节点内容
    mysqlRes = mysql_use_result(mysql);
    if (!mysqlRes)
    {
        slog_write(LL_TRACE, "5 Mysql Failed %d: %s\n", mysql_errno(mysql),
                   mysql_error(mysql));
        exit(1);
    }

    gettimeofday(&tv, NULL);

    lnode.addr = 5;             // 随便赋一个大于2的数
    while ((mysqlRow = mysql_fetch_row(mysqlRes)) != NULL)
    {
        if (*mysqlRow[0] == 0)
            continue;

        strcpy(lnode.domain, mysqlRow[0]);
        lnode.robotBlkNum = atoi(mysqlRow[1]);
        updatetime = atoi(mysqlRow[2]);

        if (tv.tv_sec - updatetime > EXPIRE_TIME)
            continue;

        // robot内存块的append
        if ((ROBOT_LEN - rbcount) < lnode.robotBlkNum)
            sdns_robot_append();

        lnode.robotPosition = robot + rbcount;

        //copy mysql robot info into memory
        memcpy(robot + rbcount, mysqlRow[3],
               lnode.robotBlkNum * ROBOT_BLK_SIZE);

        //aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
        //      printf("aaaaaaa = %s\n",lnode.domain);
        //      printf("qqqqqqq = %d\n",i=(*(short*)(robot+rbcount)->robotBlk));
        //      printf("aaaaaaa = %s\n",((char*)(robot+rbcount)->robotBlk)+2);          
        //      printf("qqqqqqq = %d\n",(*(short*)((robot+rbcount)->robotBlk+i+2)));
        //      printf("aaaaaaa = %s\n",((char*)(robot+rbcount)->robotBlk)+i+2+2);              
        //      exit(1);
        //aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa                  

        //robot位置记录
        rbcount += lnode.robotBlkNum;

        //记录总数
        allrobotcount += lnode.robotBlkNum;

        //hash key计算
        hashkey = sdns_hash_key(&lnode);
        ret = sdns_hash_classify(lnode.domain);
        if (hashkey < 0 || ret == -1)
        {
            slog_write(LL_TRACE, "Here occur a error ^_^!!!\n");
            continue;
        }
        if (lnode.domain[0] != '\0' && lnode.robotBlkNum != 0)
        {
            sdns_hash_load(hash + HASH_LEN * ret, &lnode, hashkey);
            i++;
        }
    }

    slog_write(LL_TRACE, "LOAD ROBOT into MEMORY num: %d", i);

    //释放空间
    mysql_free_result(mysqlRes);

    return 0;
}


/*-----------------------------------------------
 * load mysql dnscache table into SDNS cache
 *
 * 将mysql的domain及ip等信息导入内存
 *----------------------------------------------*/

int sdns_mysql_load()
{
    int i = 0, ret, hashkey, mode, queryLen;
    int inverseHashKey, inverseRet;
    struct timeval tv;
    struct in_addr sin;
    MYSQL *mysql = NULL;
    MYSQL_RES *mysqlRes = NULL;
    MYSQL_ROW mysqlRow;
    NODE lnode, iNode;

    // 连接数据库
    if (!(mysql = mysql_init(NULL))
        || !mysql_real_connect(mysql, "localhost", "root", "Z8nriT6B",
                               "sdnsbak", 0, NULL, 0))
        //|| !mysql_real_connect(mysql,"localhost","root","9YBThKaC","sdnsbak",0,NULL,0))
    {
        slog_write(LL_TRACE, "6 Mysql Failed %d: %s\n", mysql_errno(mysql),
                   mysql_error(mysql));
        exit(1);
    }
    SDNSmysql = mysql;

    queryLen = strlen("select * from dnscache");
    mysql_real_query(mysql, "select * from dnscache", queryLen);

    // 导入 SDNS cache 节点内容
    mysqlRes = mysql_use_result(mysql);
    if (!mysqlRes)
    {
        slog_write(LL_TRACE, "7 Mysql Failed %d: %s\n", mysql_errno(mysql),
                   mysql_error(mysql));
        exit(1);
    }
    //次过程在robot load之前，要先将所有robot项清零
    lnode.robotBlkNum = 0;
    lnode.robotPosition = 0;
    iNode.robotBlkNum = 0;
    iNode.robotPosition = 0;

    gettimeofday(&tv, NULL);

    while ((mysqlRow = mysql_fetch_row(mysqlRes)) != NULL)
    {
        if (*mysqlRow[0] == 0 || atoi(mysqlRow[5]) == 1)
            continue;

        strcpy(lnode.domain, mysqlRow[0]);      //domain
        inet_pton(AF_INET, mysqlRow[1], &lnode.addr);   //ip
        if (lnode.addr == 0)
            continue;

        mode = 0;               //mode
        lnode.lastUseTime = atoi(mysqlRow[3]);  //last use time
        lnode.updateTime = atoi(mysqlRow[4]);   //update time

        if (tv.tv_sec - lnode.updateTime > EXPIRE_TIME)
            continue;

        hashkey = sdns_hash_key(&lnode);
        ret = sdns_hash_classify(lnode.domain);
        if (hashkey < 0 || ret == -1)
        {
            slog_write(LL_TRACE, "Here occur a error ^_^!!!");
            continue;
        }
        if (lnode.addr > 1024 && lnode.domain[0] != '\0')
        {
            if (mode == 0)
            {
                //正解项载入
                sdns_hash_load(hash + HASH_LEN * ret, &lnode, hashkey);
            } else
            {
                //反解项载入
                sin.s_addr = lnode.addr;
                strcpy(iNode.domain, (char *) inet_ntoa(sin));
                iNode.addr = lnode.addr;
                iNode.lastUseTime = lnode.lastUseTime;
                iNode.updateTime = lnode.updateTime;

                inverseHashKey = sdns_hash_key(&iNode);
                if (inverseHashKey < 0)
                {
                    slog_write(LL_NOTICE, "hash key occur bt error @_@");
                    continue;
                }
                inverseRet = sdns_hash_classify(iNode.domain);
                if (inverseRet > 4 || inverseRet < 0)
                {
                    slog_write(LL_NOTICE, "get a error in url creat @_@");
                    continue;
                }

                sdns_hash_insert(hash + HASH_LEN * ret, &lnode, hashkey);
                sdns_hash_insert(hash + HASH_LEN * inverseRet, &iNode,
                                 inverseHashKey);

                lnode.alias->alias = iNode.alias;
                iNode.alias->alias = lnode.alias;
            }
        }
        i++;
    }
    slog_write(LL_TRACE,
               "Success to load %d domain into cache from MYSQL!^_^", i);

    //释放内存
    mysql_free_result(mysqlRes);

    // load robot must after load cache node
    sdns_robot_load(mysql);

#ifdef IPNOTE
    // load ipnote
    sdns_mysql_ipnote_init(mysql);
#endif

    //不能关!!!
    //mysql_close(mysql);

    return 0;
}

//------------------- END --------------------
