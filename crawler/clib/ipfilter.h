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
 * ÔØÈëipÁĞ±í£¬½«ipÁĞ±í´ÓÎÄ¼ş¶Áµ½ÄÚ´æÖĞ¡£¸Ãº¯Êı·ÑÊ±½Ï³¤¡£
 * ·µ»ØÖµ£º     
 *              ³É¹¦·µ»Ø0£»Ê§°Ü·µ»Ø-1(Ê§°ÜÊ±½«listÖØĞÂ³õÊ¼»¯,²¢²»ÊÍ·Ålist)
 * ²ÎÊı£º
 *              pathname - ipÁĞ±íÎÄ¼ş
 *              list - ipÁĞ±íÍ·Ö¸Õë
 */
int iplist_load(const u_char *pathname);

/*
 * ³õÊ¼»¯ipÁĞ±í¡£
 * ²ÎÊı£º
 *              list - ipÁĞ±íÍ·Ö¸Õ
 *              listlen - ipÁĞ±íµÄ³¤¶È
 */
void iplist_init();

/*
 * ²éÕÒ£¬È·¶¨¸ø¶¨ipÊÇ·ñÔÚipÁĞ±íÖĞ¡£
 * ·µ»ØÖµ£º
 *              ³É¹¦·µ»Ø0£»Ê§°Ü·µ»Ø-1
 * ²ÎÊı£º
 *              iplist - ipÁĞ±íÍ·Ö¸Õë
 *              ip - p¶ş½øÖÆÍøÂç×Ö½ÚĞòµÄ´ıÅĞ¶¨ip
 */
int iplist_search(unsigned long netlong);

/*
 * Ğ¶ÔØipÁĞ±í£¬ÊÍ·ÅipËùÕ¼ÄÚ´æ¡£
 * ²ÎÊı£º       
 *              iplist - ipÁĞ±íÍ·Ö¸Õë
 */     
void iplist_destroy();

#endif  
