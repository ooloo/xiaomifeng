#include <stdio.h>
#include <string.h>
#include <time.h>

/**< 描述字段标识.*/
typedef enum {
	EN_TRADE_FID_URL		= 0,
	EN_TRADE_FID_TITLE		= 1,
	EN_TRADE_FID_GAME		= 2,	// 游戏区服
	EN_TRADE_FID_PRICE		= 3,	
	EN_TRADE_FID_MODE		= 4,	// 交易方式(线上,线下,交易地区)
	EN_TRADE_FID_PUBTIME	= 5,	
	EN_TRADE_FID_LEFTTIME	= 6,
	EN_TRADE_FID_TYPE		= 7,	// 交易类型(买,卖…)
	EN_TRADE_FID_USERRANK	= 8,	// 用户等级
	EN_TRADE_FID_KIND		= 9,	// 交易种类(装备,账号,代练,货币,游戏卡…)
	EN_TRADE_FID_NUMBER		= 10,	// 剩余件数
	EN_TRADE_FID_STAT		= 11,	// 交易状态
	EN_TRADE_FID_USERINFO	= 12,	// 联系方式
}tradeFieldId;

/**< 描述字段属性.*/
typedef enum {
	EN_TRADE_FPR_NONE	= 1 << 0,
	EN_TRADE_FPR_FILTER = 1 << 1,
	EN_TRADE_FPR_INDEX	= 1 << 2,
	EN_TRADE_FPR_SUM	= 1 << 3,
	EN_TRADE_FPR_RANK	= 1 << 4,
}tradeFieldProp;

// Link,交易名称,游戏区服,交易价格,交易方式,发布时间,剩余时间,信息类型,用户等级,物品种类,剩余数量,交易状态,联系方式
#define MAX_FIELD_NUM	13
#define MAX_FIELD_LEN	256

typedef enum
{
	BUY = 0,
	SELL = 1,
	TYPEOTHER = 2,
}TRADETYPE;

//交易种类
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

