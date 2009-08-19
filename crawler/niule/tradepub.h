#include <stdio.h>
#include <string.h>
#include <time.h>

/**< �����ֶα�ʶ.*/
typedef enum {
	EN_TRADE_FID_URL		= 0,
	EN_TRADE_FID_TITLE		= 1,
	EN_TRADE_FID_GAME		= 2,	// ��Ϸ����
	EN_TRADE_FID_PRICE		= 3,	
	EN_TRADE_FID_MODE		= 4,	// ���׷�ʽ(����,����,���׵���)
	EN_TRADE_FID_PUBTIME	= 5,	
	EN_TRADE_FID_LEFTTIME	= 6,
	EN_TRADE_FID_TYPE		= 7,	// ��������(��,����)
	EN_TRADE_FID_USERRANK	= 8,	// �û��ȼ�
	EN_TRADE_FID_KIND		= 9,	// ��������(װ��,�˺�,����,����,��Ϸ����)
	EN_TRADE_FID_NUMBER		= 10,	// ʣ�����
	EN_TRADE_FID_STAT		= 11,	// ����״̬
	EN_TRADE_FID_USERINFO	= 12,	// ��ϵ��ʽ
}tradeFieldId;

/**< �����ֶ�����.*/
typedef enum {
	EN_TRADE_FPR_NONE	= 1 << 0,
	EN_TRADE_FPR_FILTER = 1 << 1,
	EN_TRADE_FPR_INDEX	= 1 << 2,
	EN_TRADE_FPR_SUM	= 1 << 3,
	EN_TRADE_FPR_RANK	= 1 << 4,
}tradeFieldProp;

// Link,��������,��Ϸ����,���׼۸�,���׷�ʽ,����ʱ��,ʣ��ʱ��,��Ϣ����,�û��ȼ�,��Ʒ����,ʣ������,����״̬,��ϵ��ʽ
#define MAX_FIELD_NUM	13
#define MAX_FIELD_LEN	256

typedef enum
{
	BUY = 0,
	SELL = 1,
	TYPEOTHER = 2,
}TRADETYPE;

//��������
typedef enum
{
	EQUIPMENT = 0,
	CHARACTER = 1,
	COIN = 2,
	CARDS = 3,
	KINDOTHER = 4,
}TRADEKIND;

typedef struct
{
	char trLink[MAX_FIELD_LEN];
	char trTitle[MAX_FIELD_LEN];
	char trGame[MAX_FIELD_LEN];
	float trPrice;
	char trMode[MAX_FIELD_LEN];
	time_t trPubTime;
	char trLeftTime[MAX_FIELD_LEN];
	TRADETYPE trType;
	char trUserRank[MAX_FIELD_LEN];
	TRADEKIND trKind;
	int trNumber;
	char trStat[MAX_FIELD_LEN];
	char trUserInfo[MAX_FIELD_LEN];
}TRADEDATA;

typedef struct
{
	char search[1024];
	unsigned page;
	char sort[MAX_FIELD_LEN];
	unsigned game;
	unsigned type;
	unsigned mode;
	unsigned pn;
}QUERYNODE;

#define MAX_WORD_NUM 128
#define MAX_ITEM_LEN 2048

int convert_trade_data(char *src, TRADEDATA *node);

int query_parse(char *query, QUERYNODE *node);

