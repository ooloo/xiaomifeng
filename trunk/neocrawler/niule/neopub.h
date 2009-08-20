#include <stdio.h>
#include <time.h>

#define MAX_HTML_LEN 1024*1024
#define MAX_ITEM_LEN 1024
#define MAX_WORD_NUM 256

/*
typedef struct
{
	time_t m_tTimeIns;
	time_t m_tTimeUpd;
	int m_iOp;
	int m_iType;
	int m_iBBSId;
	int m_iLenUrl;
	int m_iLenTitle;
	int m_iLenHtml;
}html_entry;
*/
typedef struct
{
	char url[MAX_ITEM_LEN];
	char title[MAX_ITEM_LEN];
	time_t insertTime;
	unsigned wordNum;
	unsigned summaryLen;
}INDEXDATA;

