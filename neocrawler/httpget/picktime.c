#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "picktime.h"

const char yearStr[] = "年";
const char monthStr[] = "月";
const char dayStr[] = "日";

const int WEIGHT_BASE = 100;
const int WEIGHT_HMS = 80;
const int HINT_DIST = 50;       // 提示字串的最大距离
const int TAG_DIST = 200;       // 时间字符串所在两个标签间的最大距离

int match_char(char **p, char c)
{
    assert(p && *p);

    if (**p && **p == c)
    {
        ++*p;
        return 1;
    } else
        return 0;
}

int match_digit(char **p, int *out)
{
    assert(p && *p && out);

    if (**p && **p >= '0' && **p <= '9')
    {
        *out = **p - '0';
        ++*p;
        return 1;
    } else
        return 0;
}

int match_2chars(char **p, const char *s)
{
    assert(p && *p && s);

    if (**p && **p == s[0] && *(*p + 1) && *(*p + 1) == s[1])
    {
        *p += 2;
        return 1;
    } else
        return 0;
}

int match_digits(char **p, int *out)
{
    assert(p && *p && out);

    if (match_digit(p, out))
    {
        while (**p && **p >= '0' && **p <= '9')
        {
            *out = *out * 10 + (**p - '0');
            ++*p;
        }
        return 1;
    } else
        return 0;
}

int match_hms(char **p, int *hour, int *min, int *sec)
{
    char *q = *p, *t;

    assert(p && *p && hour && min && sec);

    if (!match_char(&q, ' '))
        match_2chars(&q, "　");

    if (match_digits(&q, hour)
        && *hour >= 0
        && *hour <= 23
        && match_char(&q, ':')
        && match_digits(&q, min) && *min >= 0 && *min <= 59)
    {
        t = q;

        if (match_char(&q, ':')
            && match_digits(&q, sec) && *sec >= 0 && *sec <= 59)
            *p = q;
        else
        {
            *p = t;
            *sec = 0;
        }

        return 1;
    } else
        return 0;
}

int pick_a_time(char *text)
{
    char *pstart, *p, *q, sepChar;
    struct tm ttm;
    int tmp;
    int curTime;

    pstart = text;
    curTime = 0;

    // 找所有有效的时间串，取权值最大的一个
    while (1)
    {
        // 找到一个时间
        p = strstr(pstart, "200");
        q = strstr(pstart, "199");
        if (p)
        {
            ttm.tm_year = 100;
            if (q && p > q)
            {
                p = q;
                ttm.tm_year = 90;
            }
        } else
        {
            if (!(p = q))
                break;
            ttm.tm_year = 90;
        }
        // 目前p指向可能的时间串的开头

        q = p + 3;
        if (!match_digit(&q, &tmp))
        {
            pstart = q;
            continue;
        }
        ttm.tm_year += tmp;

        if (*q == '-' || *q == '/' || *q == '.')
        {
            sepChar = *q;

            q++;
            if (!match_digits(&q, &(ttm.tm_mon))
                || ttm.tm_mon < 1
                || ttm.tm_mon > 12
                || !match_char(&q, sepChar)
                || !match_digits(&q, &(ttm.tm_mday))
                || ttm.tm_mday < 1 || ttm.tm_mday > 31)
            {
                pstart = q;
                continue;
            }

            if (!match_hms
                (&q, &(ttm.tm_hour), &(ttm.tm_min), &(ttm.tm_sec)))
                ttm.tm_hour = ttm.tm_min = ttm.tm_sec = 0;
        } else
        {
            if (!match_2chars(&q, yearStr))
            {
                pstart = q;
                continue;
            } else
            {
                match_char(&q, ' ');
                if (!match_digits(&q, &(ttm.tm_mon)) || ttm.tm_mon < 1
                    || ttm.tm_mon > 12 || !match_2chars(&q, monthStr))
                {
                    pstart = q;
                    continue;
                } else
                {
                    match_char(&q, ' ');
                    if (!match_digits(&q, &(ttm.tm_mday))
                        || ttm.tm_mday < 1
                        || ttm.tm_mday > 31 || !match_2chars(&q, dayStr))
                    {
                        pstart = q;
                        continue;
                    }
                }
            }

            if (!match_hms
                (&q, &(ttm.tm_hour), &(ttm.tm_min), &(ttm.tm_sec)))
                ttm.tm_hour = ttm.tm_min = ttm.tm_sec = 0;
        }
        ttm.tm_mon--;
        ttm.tm_isdst = 0;
        //ttm.tm_gmtoff=0;
        curTime = (int) mktime(&ttm);
        break;
    }

    if (curTime > 0)
    {
        return curTime;
    }
    return 0;
}


/*
 * 从网页HTML代码中提取发布时间
 * 返回值：
 *		>=0 - 成功，返回发布时间的秒数
 *		-1 - 失败
 */
int get_webpage_time(char *page)
{
    char *pstart, *p, *q, sepChar;
    struct tm ttm;
    int tmp;
    int ttime, weight;
    int curTime, curWeight;
    char *paraLeft, *paraRight;
    char tmpCh;
    char *hint;                 // 提示信息的位置，比如“发布时间”就是一个提示信息
    time_t now = time(NULL);;

    assert(page);

    // 让pstart指向HTML文档的BODY域开头（没有BODY以文档开头代替）
    if (!(pstart = (char *) strcasestr(page, "<body")))
    {
        pstart = page;
    }
    assert(pstart);

    // 找所有有效的时间串，取权值最大的一个
    for (ttime = -1, weight = -1;;)
    {
        curWeight = WEIGHT_BASE;

        // 找到一个时间
        p = strstr(pstart, "200");
        q = strstr(pstart, "199");
        if (p)
        {
            ttm.tm_year = 100;
            if (q && p > q)
            {
                p = q;
                ttm.tm_year = 90;
            }
        } else
        {
            if (!(p = q))
                break;
            ttm.tm_year = 90;
        }
        // 目前p指向可能的时间串的开头

        q = p + 3;
        if (!match_digit(&q, &tmp))
        {
            pstart = q;
            continue;
        }
        ttm.tm_year += tmp;

        if (*q == '-' || *q == '/' || *q == '.')
        {
            sepChar = *q;

            q++;
            if (!match_digits(&q, &(ttm.tm_mon))
                || ttm.tm_mon < 1
                || ttm.tm_mon > 12
                || !match_char(&q, sepChar)
                || !match_digits(&q, &(ttm.tm_mday))
                || ttm.tm_mday < 1 || ttm.tm_mday > 31)
            {
                pstart = q;
                continue;
            }

            if (!match_hms
                (&q, &(ttm.tm_hour), &(ttm.tm_min), &(ttm.tm_sec)))
                ttm.tm_hour = ttm.tm_min = ttm.tm_sec = 0;
            else
                curWeight += WEIGHT_HMS;
        } else
        {
            if (!match_2chars(&q, yearStr))
            {
                pstart = q;
                continue;
            } else
            {
                match_char(&q, ' ');
                if (!match_digits(&q, &(ttm.tm_mon)) || ttm.tm_mon < 1
                    || ttm.tm_mon > 12 || !match_2chars(&q, monthStr))
                {
                    pstart = q;
                    continue;
                } else
                {
                    match_char(&q, ' ');
                    if (!match_digits(&q, &(ttm.tm_mday))
                        || ttm.tm_mday < 1
                        || ttm.tm_mday > 31 || !match_2chars(&q, dayStr))
                    {
                        pstart = q;
                        continue;
                    }
                }
            }

            if (!match_hms
                (&q, &(ttm.tm_hour), &(ttm.tm_min), &(ttm.tm_sec)))
                ttm.tm_hour = ttm.tm_min = ttm.tm_sec = 0;
            else
                curWeight += WEIGHT_HMS;
        }
/*
		tmp=*q;
		*q=0;
		printf("\n%s\n",p);
		*q=tmp;
*/
        ttm.tm_mon--;
        ttm.tm_isdst = 0;
        //ttm.tm_gmtoff=0;
        curTime = (int) mktime(&ttm);
        assert(curTime != -1);

        // 扔掉比当前时间还大的时间
        if (curTime > now)
        {
            pstart = p + 1;
            continue;
        }
        // 找到当前时间串所在的标签段落
        // 找开头
        for (paraLeft = p - 1;
             *paraLeft && *paraLeft != '>' && *paraLeft != '<'; paraLeft--)
            ;

        if (!*paraLeft)
            paraLeft = pstart;
        else if (*paraLeft == '<')
        {
            pstart = p + 1;
            continue;
        } else
            paraLeft++;

        // 找结尾
        paraRight = strchr(q, '<');
        if (paraRight)
        {
            tmpCh = *paraRight;
            *paraRight = 0;

            if (strchr(q, '>'))
            {
                *paraRight = tmpCh;
                pstart = p + 1;
                continue;
            }
        }
        // 时间所在字段过长放弃
        if ((int) strlen(paraLeft) > TAG_DIST)
        {
            pstart = p + 1;
            continue;
        }

        printf("%s\n", paraLeft);
        // 现在各变量指向的位置如下：
        // p - 时间串开头
        // q - 时间串末尾的下一个字符
        // paraLeft - 时间串所在的标签段落开头
        // paraRight - 时间串所在的标签段落末尾的下一个字符（或者NULL）


        // 调整权值
        if ((hint = strstr(paraLeft, "发布时间")) && hint < p
            && p - hint < HINT_DIST)
            curWeight += 100;

        if ((hint = strstr(paraLeft, "发帖时间")) && hint < p
            && p - hint < HINT_DIST)
            curWeight += 100;

        if ((hint = strstr(paraLeft, "发表")) && hint < p
            && p - hint < HINT_DIST)
            curWeight += 90;

        if ((hint = strstr(paraLeft, "post")) && hint < p
            && p - hint < HINT_DIST)
            curWeight += 90;

        if ((hint = strstr(paraLeft, "发行")) && hint < p
            && p - hint < HINT_DIST)
            curWeight -= 50;

        if ((hint = strstr(paraLeft, "现在")) && hint < p
            && p - hint < HINT_DIST)
            curWeight -= 100;

        if ((hint = strstr(paraLeft, "注册")) && hint < p
            && p - hint < HINT_DIST)
            curWeight -= 200;

        if (((hint = strstr(paraLeft, "新闻来源"))
             || (hint = strstr(paraLeft, "稿源")))
            && abs((int) (p - hint)) < HINT_DIST)
            curWeight += 90;

        if (((hint = strstr(paraLeft, "新闻网"))
             || (hint = strstr(paraLeft, "晨报"))
             || (hint = strstr(paraLeft, "早报"))
             || (hint = strstr(paraLeft, "日报"))
             || (hint = strstr(paraLeft, "晚报"))
             || (hint = strstr(paraLeft, "信息港"))
             || (hint = strstr(paraLeft, "在线")))
            && abs((int) (p - hint)) < HINT_DIST)
            curWeight += 50;

        if (((hint = strstr(paraLeft, "编辑"))
             || (hint = strstr(paraLeft, "责编"))
             || (hint = strstr(paraLeft, "责任编辑"))
             || (hint = strstr(paraLeft, "作者")))
            && abs((int) (p - hint)) < HINT_DIST)
            curWeight += 50;

        if (((hint = strstr(paraLeft, "【"))
             || (hint = strstr(paraLeft, "】"))
             || (hint = strstr(paraLeft, "〖"))
             || (hint = strstr(paraLeft, "〗")))
            && abs((int) (p - hint)) < HINT_DIST)
            curWeight += 40;

        if (((hint = strstr(paraLeft, "("))
             || (hint = strstr(paraLeft, ")"))
             || (hint = strstr(paraLeft, "（"))
             || (hint = strstr(paraLeft, "）")))
            && abs((int) (p - hint)) < HINT_DIST)
            curWeight += 20;

        if (((hint = strstr(paraLeft, "来源"))
             || (hint = strstr(paraLeft, "时间")))
            && abs((int) (p - hint)) < HINT_DIST)
            curWeight += 10;

        if ((hint = strstr(paraLeft, "http://"))
            && abs((int) (p - hint)) < HINT_DIST)
            curWeight += 10;

        if (((hint = strstr(paraLeft, "网"))
             || (hint = strstr(paraLeft, "报"))
             || (hint = strstr(paraLeft, "社"))
             || (hint = strstr(paraLeft, "讯")))
            && abs((int) (p - hint)) < HINT_DIST)
            curWeight += 10;

        // 恢复
        if (paraRight)
            *paraRight = tmpCh;

        printf("curWeight : %d\n", curWeight);
        // 比较权值，取权值更大的时间值
        if (curWeight > weight)
        {
            ttime = curTime;
            weight = curWeight;
        } else if (curWeight == weight)
        {
            if (curTime > ttime)
                ttime = curTime;
        }

        pstart = p + 1;
    }

    if (ttime > 0)
        return ttime;
    return -1;
}

/*
 * 从网页HTML代码中提取发布时间
 * 返回值：
 *		>=0 - 成功，返回发布时间的秒数
 *		-1 - 失败
 */
int get_webpage_time2(char *page)
{
    char *pstart, *p, *q, sepChar;
    int curTime = -1;
    struct tm ttm;
    int tmp;

    assert(page);

    for (pstart = page;;)
    {
        // 找到一个时间
        p = strstr(pstart, "200");
        q = strstr(pstart, "199");
        if (p)
        {
            ttm.tm_year = 100;
            if (q && p > q)
            {
                p = q;
                ttm.tm_year = 90;
            }
        } else
        {
            if (!(p = q))
                break;
            ttm.tm_year = 90;
        }
        // 目前p指向可能的时间串的开头

        q = p + 3;
        if (!match_digit(&q, &tmp))
        {
            pstart = q;
            continue;
        }
        ttm.tm_year += tmp;

        if (*q == '-' || *q == '/' || *q == '.')
        {
            sepChar = *q;

            q++;
            if (!match_digits(&q, &(ttm.tm_mon))
                || ttm.tm_mon < 1
                || ttm.tm_mon > 12
                || !match_char(&q, sepChar)
                || !match_digits(&q, &(ttm.tm_mday))
                || ttm.tm_mday < 1 || ttm.tm_mday > 31)
            {
                pstart = q;
                continue;
            }

            if (!match_hms
                (&q, &(ttm.tm_hour), &(ttm.tm_min), &(ttm.tm_sec)))
                ttm.tm_hour = ttm.tm_min = ttm.tm_sec = 0;
        } else
        {
            if (!match_2chars(&q, yearStr)
                || !match_digits(&q, &(ttm.tm_mon))
                || ttm.tm_mon < 1
                || ttm.tm_mon > 12
                || !match_2chars(&q, monthStr)
                || !match_digits(&q, &(ttm.tm_mday))
                || ttm.tm_mday < 1
                || ttm.tm_mday > 31 || !match_2chars(&q, dayStr))
            {
                pstart = q;
                continue;
            }

            if (!match_hms
                (&q, &(ttm.tm_hour), &(ttm.tm_min), &(ttm.tm_sec)))
                ttm.tm_hour = ttm.tm_min = ttm.tm_sec = 0;
        }

        tmp = *q;
        *q = 0;
        printf("\n%s\n", p);
        *q = tmp;

        ttm.tm_mon--;
        ttm.tm_isdst = 0;
        //ttm.tm_gmtoff=0;
        curTime = (int) mktime(&ttm);
        assert(curTime != -1);

        return curTime;
    }

    return -1;
}

/* 把指定时间转换为当天00:00:00的秒数 */
/*int time2day(int t)
{
	struct tm ttm;
	int tmp;

	localtime_r((const time_t*)&t,&ttm);
	ttm.tm_hour=ttm.tm_min=ttm.tm_sec=0;
	tmp=mktime(&ttm);
	assert(tmp!=-1);

	return tmp;
}*/

/*
 * 以抓取时间为参考，从网页HTML代码中提取发布时间
 * 返回值：
 *		发布时间的秒数
 */
int get_pub_time(char *page, int crawTime)
{
    int wtime, t1, t2;

    assert(page && crawTime > 0);

    wtime = get_webpage_time(page);
    if (wtime == -1)
        return crawTime;

//      t1=time2day(wtime);
//      t2=time2day(crawTime);
    t1 = wtime;
    t2 = crawTime;
    if (t1 > t2)
        return crawTime;

    if (t1 == t2 && t1 == wtime)
        return crawTime;

    return wtime;
}

/*
int main(int argc, char **argv)
{
	int len;
	char html[512*1024]={0};
	struct tm ttm;
	time_t ctime;

	if(argc != 2)
		exit(1);

	FILE *fp;
	fp = fopen(argv[1], "r");
	len = fread(html, 1, 510*1024, fp);
	fclose(fp);

	html[len] = 0;

	printf("htmlLen: %d\n", strlen(html));
	ctime = get_pub_time(html, time(NULL));
	printf("htmlLen: %d\n", strlen(html));

	localtime_r(&ctime,&ttm);
	printf("%d.%02d.%02d.%02d.%02d.%02d\n",
			 ttm.tm_year+1900,ttm.tm_mon+1,ttm.tm_mday,ttm.tm_hour,ttm.tm_min,ttm.tm_sec);

	return 0;
}
*/
