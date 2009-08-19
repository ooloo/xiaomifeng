#ifndef PREDEFINE_H_
#define PREDEFINE_H_
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef WIN32_LEAN_AND_MEAN	
   #include <Windows.h>
#else
   #include <sys/mman.h>
   #include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
/**********************************************************/
#define USERAGENT "Wget/1.10.2"//USERAGENT "Wget/1.10.2"
#define ACCEPT "*/*"
#define ACCEPTLANGUAGE "zh-cn,zh;q=0.5"
#define ACCEPTENCODING "gzip,deflate"
#define ACCEPTCHARSET "gb2312,utf-8;q=0.7,*;q=0.7"
#define KEEPALIVE "300"
#define CONNECTION "close"
#define CONTENTTYPE "application/x-www-form-urlencoded"
#define CONTENTLENGTH "Content-Length:"

#define MAXFILENAME 14
#define DEBUG 1
/**********************************************************/
typedef struct webnode {
	std::string host;                 /* ��ҳ���ڵ����� */
	int port;                         /* �����������ʹ�õĶ˿� */
	std::string dir;                  /* ��ҳ���ڵ�Ŀ¼ */
	std::string page;                 /* ��ҳ�ļ��� */
	std::string file;                 /* ���ر�����ļ��� */
	char IsHandled;                   /* �Ƿ���� */
	struct webnode * brother;         /* �ֵܽڵ�����ָ�� */
	struct webnode * child;           /* �ӽڵ�����ָ�� */
	int nSize;                        /* ��ҳ���ݴ�С */
} WEBNODE;


typedef struct sitadd {
	std::string HeadName;             /* ��ַ��ͷ */
	int Start;                        /* �ļ����ƿ�ʼ */
	int End;                          /* �ļ�ĩβ */
	std::string FileType;             /* �ļ������ǣ�html,asp�ȣ�*/
} SITADD;


//typedef struct ruleset {
//	std::string SigleStart;           /* ��ȡ��ʼ��־ */
//	std::string SigleEnd;             /* ��ȡ������־ */
//	int Deeply;                       /* ��� */
//	int Type;                         /* �������ͣ�1:�ı�;2:ͼƬ�� */
//	std::string FileType;             /* �ļ����ͣ�txt,gif,jpg��*/
//	struct ruleset* brother;
//	struct ruleset* child;
//} RULESET;

typedef struct bufferpool {
	char* cBuffer;
	int nSize;
} BUFFERPOOL;
const static char SINGE[3]= "@/";
#endif /*PREDEFINE_H_*/
