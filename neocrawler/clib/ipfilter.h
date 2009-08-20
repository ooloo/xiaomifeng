#ifndef IPFILTER_H
#define IPFILTER_H

#include <stdio.h>
#include <sys/types.h>

#define IP_LIST_LEN	65536

typedef struct _ipnode
{
	unsigned netlong;
	unsigned short mask;
	unsigned short region;
	struct _ipnode *next;
}IPNODE;

typedef struct
{
	struct _ipnode *ipnode;
}IPLIST;

extern IPLIST *iplist;

/*
 * ����ip�б���ip�б���ļ������ڴ��С��ú�����ʱ�ϳ���
 * ����ֵ��     
 *              �ɹ�����0��ʧ�ܷ���-1(ʧ��ʱ��list���³�ʼ��,�����ͷ�list)
 * ������
 *              pathname - ip�б��ļ�
 *              list - ip�б�ͷָ��
 */
int iplist_load(const u_char *pathname);

/*
 * ��ʼ��ip�б�
 * ������
 *              list - ip�б�ͷָ�
 *              listlen - ip�б�ĳ���
 */
void iplist_init();

/*
 * ���ң�ȷ������ip�Ƿ���ip�б��С�
 * ����ֵ��
 *              �ɹ�����0��ʧ�ܷ���-1
 * ������
 *              iplist - ip�б�ͷָ��
 *              ip - p�����������ֽ���Ĵ��ж�ip
 */
int iplist_search(unsigned long netlong);

/*
 * ж��ip�б��ͷ�ip��ռ�ڴ档
 * ������       
 *              iplist - ip�б�ͷָ��
 */     
void iplist_destroy();

#endif  
