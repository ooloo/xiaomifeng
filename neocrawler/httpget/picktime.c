#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "picktime.h"

const char yearStr[] = "��";
const char monthStr[] = "��";
const char dayStr[] = "��";

const int WEIGHT_BASE = 100;
const int WEIGHT_HMS = 80;
const int HINT_DIST = 50;       // ��ʾ�ִ���������
const int TAG_DIST = 200;       // ʱ���ַ�������������ǩ���������

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
        match_2chars(&q, "��");

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

    // ��������Ч��ʱ�䴮��ȡȨֵ����һ��
    while (1)
    {
        // �ҵ�һ��ʱ��
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
        // Ŀǰpָ����ܵ�ʱ�䴮�Ŀ�ͷ

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
 * ����ҳHTML��������ȡ����ʱ��
 * ����ֵ��
 *		>=0 - �ɹ������ط���ʱ�������
 *		-1 - ʧ��
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
    char *hint;                 // ��ʾ��Ϣ��λ�ã����硰����ʱ�䡱����һ����ʾ��Ϣ
    time_t now = time(NULL);;

    assert(page);

    // ��pstartָ��HTML�ĵ���BODY��ͷ��û��BODY���ĵ���ͷ���棩
    if (!(pstart = (char *) strcasestr(page, "<body")))
    {
        pstart = page;
    }
    assert(pstart);

    // ��������Ч��ʱ�䴮��ȡȨֵ����һ��
    for (ttime = -1, weight = -1;;)
    {
        curWeight = WEIGHT_BASE;

        // �ҵ�һ��ʱ��
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
        // Ŀǰpָ����ܵ�ʱ�䴮�Ŀ�ͷ

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

        // �ӵ��ȵ�ǰʱ�仹���ʱ��
        if (curTime > now)
        {
            pstart = p + 1;
            continue;
        }
        // �ҵ���ǰʱ�䴮���ڵı�ǩ����
        // �ҿ�ͷ
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

        // �ҽ�β
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
        // ʱ�������ֶι�������
        if ((int) strlen(paraLeft) > TAG_DIST)
        {
            pstart = p + 1;
            continue;
        }

        printf("%s\n", paraLeft);
        // ���ڸ�����ָ���λ�����£�
        // p - ʱ�䴮��ͷ
        // q - ʱ�䴮ĩβ����һ���ַ�
        // paraLeft - ʱ�䴮���ڵı�ǩ���俪ͷ
        // paraRight - ʱ�䴮���ڵı�ǩ����ĩβ����һ���ַ�������NULL��


        // ����Ȩֵ
        if ((hint = strstr(paraLeft, "����ʱ��")) && hint < p
            && p - hint < HINT_DIST)
            curWeight += 100;

        if ((hint = strstr(paraLeft, "����ʱ��")) && hint < p
            && p - hint < HINT_DIST)
            curWeight += 100;

        if ((hint = strstr(paraLeft, "����")) && hint < p
            && p - hint < HINT_DIST)
            curWeight += 90;

        if ((hint = strstr(paraLeft, "post")) && hint < p
            && p - hint < HINT_DIST)
            curWeight += 90;

        if ((hint = strstr(paraLeft, "����")) && hint < p
            && p - hint < HINT_DIST)
            curWeight -= 50;

        if ((hint = strstr(paraLeft, "����")) && hint < p
            && p - hint < HINT_DIST)
            curWeight -= 100;

        if ((hint = strstr(paraLeft, "ע��")) && hint < p
            && p - hint < HINT_DIST)
            curWeight -= 200;

        if (((hint = strstr(paraLeft, "������Դ"))
             || (hint = strstr(paraLeft, "��Դ")))
            && abs((int) (p - hint)) < HINT_DIST)
            curWeight += 90;

        if (((hint = strstr(paraLeft, "������"))
             || (hint = strstr(paraLeft, "����"))
             || (hint = strstr(paraLeft, "�籨"))
             || (hint = strstr(paraLeft, "�ձ�"))
             || (hint = strstr(paraLeft, "��"))
             || (hint = strstr(paraLeft, "��Ϣ��"))
             || (hint = strstr(paraLeft, "����")))
            && abs((int) (p - hint)) < HINT_DIST)
            curWeight += 50;

        if (((hint = strstr(paraLeft, "�༭"))
             || (hint = strstr(paraLeft, "���"))
             || (hint = strstr(paraLeft, "���α༭"))
             || (hint = strstr(paraLeft, "����")))
            && abs((int) (p - hint)) < HINT_DIST)
            curWeight += 50;

        if (((hint = strstr(paraLeft, "��"))
             || (hint = strstr(paraLeft, "��"))
             || (hint = strstr(paraLeft, "��"))
             || (hint = strstr(paraLeft, "��")))
            && abs((int) (p - hint)) < HINT_DIST)
            curWeight += 40;

        if (((hint = strstr(paraLeft, "("))
             || (hint = strstr(paraLeft, ")"))
             || (hint = strstr(paraLeft, "��"))
             || (hint = strstr(paraLeft, "��")))
            && abs((int) (p - hint)) < HINT_DIST)
            curWeight += 20;

        if (((hint = strstr(paraLeft, "��Դ"))
             || (hint = strstr(paraLeft, "ʱ��")))
            && abs((int) (p - hint)) < HINT_DIST)
            curWeight += 10;

        if ((hint = strstr(paraLeft, "http://"))
            && abs((int) (p - hint)) < HINT_DIST)
            curWeight += 10;

        if (((hint = strstr(paraLeft, "��"))
             || (hint = strstr(paraLeft, "��"))
             || (hint = strstr(paraLeft, "��"))
             || (hint = strstr(paraLeft, "Ѷ")))
            && abs((int) (p - hint)) < HINT_DIST)
            curWeight += 10;

        // �ָ�
        if (paraRight)
            *paraRight = tmpCh;

        printf("curWeight : %d\n", curWeight);
        // �Ƚ�Ȩֵ��ȡȨֵ�����ʱ��ֵ
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
 * ����ҳHTML��������ȡ����ʱ��
 * ����ֵ��
 *		>=0 - �ɹ������ط���ʱ�������
 *		-1 - ʧ��
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
        // �ҵ�һ��ʱ��
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
        // Ŀǰpָ����ܵ�ʱ�䴮�Ŀ�ͷ

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

/* ��ָ��ʱ��ת��Ϊ����00:00:00������ */
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
 * ��ץȡʱ��Ϊ�ο�������ҳHTML��������ȡ����ʱ��
 * ����ֵ��
 *		����ʱ�������
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
