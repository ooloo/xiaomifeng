#ifndef NEOMYSQL_H
#define NEOMYSQL_H


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include "slog.h"
#include "mysql.h"
#include "sdns.h"

#define MAX_DOMAIN_NUM_PER_IP 1000

extern MYSQL *SDNSmysql;
extern char *SDNSquery;


/*-----------------------------------------------
 *	addr���ݿ����
 *	����addr������ȫ��ɾ��
 *---------------------------------------------*/

void sdns_mysql_ipnote_init(MYSQL *mysql);


/*-----------------------------------------------
 * 	����һ��ip��ַ��Ӧ����domain�����
 * 	return:
 * 		-1	error
 *	     0	success
 *---------------------------------------------*/

int sdns_mysql_ipnote_query(MYSQL *mysql,char *ipdomain,char *domain);

/*-----------------------------------------------
 * insert one snds cache NODE into mysql 
 *---------------------------------------------*/

int sdns_mysql_insert(MYSQL *mysql,NODE *lnode,int mode,int ifdel);


/*-----------------------------------------------
 *	дrobot��Ϣ��MYSQL
 *	RETURN :
 *			-1 : error
 *			 0 : insert
 *			 1 : update
 *---------------------------------------------*/

int sdns_robot_write(MYSQL *mysql,char *domain,int BlkNum,ROBOT *robotBlk,char *query);


/*-----------------------------------------------
 * 	����ipnote���ݱ�
 * 	���ڷ�������domain
 *---------------------------------------------*/

int sdns_mysql_ipnote_create(MYSQL *mysql);

int sdns_mysql_ipnote_check(MYSQL *mysql,char *ipdomain);

/*-----------------------------------------------
 * load mysql robot into memory
 *---------------------------------------------*/

int sdns_robot_load(MYSQL *mysql);


/*-----------------------------------------------
 * load mysql dnscache table into SDNS cache
 *---------------------------------------------*/

int sdns_mysql_load();


#endif

//------------------ END ------------------
