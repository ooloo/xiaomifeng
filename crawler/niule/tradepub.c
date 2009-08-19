#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tradepub.h" 
#include "tradedecode.h"

int str2time_t(char *s)
{
	int pos,len,flag;
	char *p, standardtimestr[128];
	time_t now;
	struct tm tm, nm;

	// make str standard!
	while(isdigit(*s) == 0 && *s != 0)
		s++;

	standardtimestr[0] = *s;
	pos = 1; s++; flag = 0;
	while(*s != 0 && pos < 100)
	{
		if(strncmp(s, "年", 2) == 0 || strncmp(s, "月", 2) == 0)
		{
			standardtimestr[pos++] = '-';
			s += 2;
		}
		if(strncmp(s, "日", 2) == 0 || strncmp(s, "　", 2) == 0)
		{
			if(standardtimestr[pos-1] != ' ')
				standardtimestr[pos++] = ' ';
			s += 2;
		}
		if(strncmp(s, "点", 2) == 0 || strncmp(s, "时", 2) == 0 || strncmp(s, "分", 2) == 0)
		{
			standardtimestr[pos++] = ':';
			s += 2;
		}
		if(*s == '/')
		{
			standardtimestr[pos++] = '-';
			s += 1;
		}
		if(standardtimestr[pos-1] != ' ' || *s != ' ')
		{
			standardtimestr[pos++] = *s;
		}
		s++;
	}

	standardtimestr[pos] = 0;

	p = standardtimestr;
	while(*p != 0)
	{
		if(*p == '-')
			flag++;
		p++;
	}

	time(&now);
	localtime_r(&now, &nm);

	bzero(&tm, sizeof(struct tm));
	tm.tm_year = nm.tm_year;

	if(flag == 1)
	{
		strptime(standardtimestr, "%m-%d %H:%M:%S", &tm);
	}
	else if(flag == 2 && atoi(standardtimestr) < 100)
	{
		strptime(standardtimestr, "%y-%m-%d %H:%M:%S", &tm);
	}
	else
	{
		strptime(standardtimestr, "%Y-%m-%d %H:%M:%S", &tm);
	}

	tzset();
	printf("%s ==== %d\n", standardtimestr, mktime(&tm));
	now = now - timezone;
	if(mktime(&tm) - timezone > now + 3600)
		return now;
	else
		return (mktime(&tm) - timezone);
}

int convert_trade_data(char *src, TRADEDATA *node)
{
	int i;
	char *tok;

	bzero(node, sizeof(TRADEDATA));
	//printf("---------------------------------------------\n");

	i = 0;
	tok=strtok(src,"\t");
	while(tok!=NULL)
	{
		i++;
		switch(i)
		{
		case 1:
			snprintf(node->trLink, MAX_FIELD_LEN, "%s", tok);
			//	printf("link\t: %s\n", tok);
			break;
		case 2:
			snprintf(node->trTitle, MAX_FIELD_LEN, "%s", tok);
			//	printf("title\t: %s\n", tok);
			break;
		case 3:
			snprintf(node->trGame, MAX_FIELD_LEN, "%s", tok);
			//	printf("game\t: %s\n", tok);
			break;
		case 4:
			node->trPrice = (float)atof(tok);
			//	printf("price\t: %f\n", node->trPrice);
			break;
		case 5:
			snprintf(node->trMode, MAX_FIELD_LEN, "%s", tok);
			//	printf("mode\t: %s\n", tok);
			break;
		case 6:
			node->trPubTime = str2time_t(tok);
			//	printf("pubtime : %d\n", node->trPubTime);
			break;
		case 7:
			snprintf(node->trLeftTime, MAX_FIELD_LEN, "%s", tok);
			//	printf("left\t: %s\n", tok);
			break;
		case 8:
			if(strstr(tok, "出") || strstr(tok, "卖"))
				node->trType = 1;
			else
				node->trType = 0;
			//	printf("type\t: %d\n", node->trType);
			break;
		case 9:
			snprintf(node->trUserRank, MAX_FIELD_LEN, "%s", tok);
			//	printf("rank\t: %s\n", tok);
			break;
		case 10:
			if(strstr(tok, "装备"))
				node->trKind = 0;
			else if(strstr(tok, "帐号"))
				node->trKind = 1;
			else if(strstr(tok, "币"))
				node->trKind = 2;
			else if(strstr(tok, "卡"))
				node->trKind = 3;
			else
				node->trKind = 4;
			//	printf("kind\t: %d\n", node->trKind);
			break;
		case 11:
			node->trNumber = atoi(tok);
			//	printf("numner\t: %d\n", node->trNumber);
			break;
		case 12:
			snprintf(node->trStat, MAX_FIELD_LEN, "%s", tok);
			//	printf("stat\t: %s\n", tok);
			break;
		case 13:
			snprintf(node->trUserInfo, MAX_FIELD_LEN, "%s", tok);
			//	printf("info\t: %s\n", tok);
			break;
		default:
			break;
		}
		tok=strtok(NULL,"\t");
	}

	if(i == MAX_FIELD_NUM)
		return 0;
	else
		return -1;
}

int query_parse(char *query, QUERYNODE *node)
{
	char *p,*t,*tok,sea[1024],tmp[2]={0};

	bzero(node, sizeof(QUERYNODE));

	strcpy(sea, query);

	tok=strtok(sea,"&");
	while(tok!=NULL)
	{
		if((p=strstr(tok, "search=")) != NULL)
		{
			p = p + strlen("search=");
			t=p;
			while(*t)
			{
				if(*t == '+')
					*t = ' ';
				t++;
			}
			decodeUrl(p);
			snprintf(node->search, 1024, "%s", p);
		}
		else if((p=strstr(tok, "page=")) != NULL)
		{
			p = p + strlen("page=");
			node->page = atoi(p);
		}
		else if((p=strstr(tok, "sort=")) != NULL)
		{
			p = p + strlen("sort=");
			snprintf(node->sort, MAX_FIELD_LEN, "%s", p);
		}
		else if((p=strstr(tok, "game=")) != NULL)
		{
			p = p + strlen("game=");
			node->game = atoi(p);
		}
		else if((p=strstr(tok, "type=")) != NULL)
		{
			p = p + strlen("type=");
			node->type = atoi(p);
		}
		else if((p=strstr(tok, "mode=")) != NULL)
		{
			p = p + strlen("mode=");
			node->mode = atoi(p);
		}
		else if((p=strstr(tok, "pn=")) != NULL)
		{
			p = p + strlen("pn=");
			node->pn = atoi(p);
		}

		tok=strtok(NULL,"&");
	}

	return 0;
}
