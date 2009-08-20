#ifndef PICKTIME_H
#define PICKTIME_H

/*
 * 从网页HTML代码中提取发布时间
 * 返回值：
 *		>=0 - 成功，返回发布时间的秒数
 *		-1 - 失败
 *	参数：
 *		page - 网页正文
 */
extern int get_webpage_time(char *page);
extern int get_webpage_time2(char *page);

extern int pick_a_time(char *text);


/* 把指定时间转换为当天00:00:00的秒数 */
extern int time2day(int t);

/*
 * 以抓取时间为参考，从网页HTML代码中提取发布时间
 * 返回值：
 *		发布时间的秒数
 *	参数：
 *		page - 网页正文
 *		crawtime - 抓取时间
 */
extern int get_pub_time(char *page,int crawTime);

#endif


