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
	std::string host;                 /* 网页所在的主机 */
	int port;                         /* 网络服务器所使用的端口 */
	std::string dir;                  /* 网页所在的目录 */
	std::string page;                 /* 网页文件名 */
	std::string file;                 /* 本地保存的文件名 */
	char IsHandled;                   /* 是否处理过 */
	struct webnode * brother;         /* 兄弟节点链表指针 */
	struct webnode * child;           /* 子节点链表指针 */
	int nSize;                        /* 网页数据大小 */
} WEBNODE;


typedef struct sitadd {
	std::string HeadName;             /* 网址字头 */
	int Start;                        /* 文件名称开始 */
	int End;                          /* 文件末尾 */
	std::string FileType;             /* 文件类型是（html,asp等）*/
} SITADD;


//typedef struct ruleset {
//	std::string SigleStart;           /* 截取开始标志 */
//	std::string SigleEnd;             /* 截取结束标志 */
//	int Deeply;                       /* 深度 */
//	int Type;                         /* 数据类型（1:文本;2:图片） */
//	std::string FileType;             /* 文件类型（txt,gif,jpg）*/
//	struct ruleset* brother;
//	struct ruleset* child;
//} RULESET;

typedef struct bufferpool {
	char* cBuffer;
	int nSize;
} BUFFERPOOL;
const static char SINGE[3]= "@/";
#endif /*PREDEFINE_H_*/
