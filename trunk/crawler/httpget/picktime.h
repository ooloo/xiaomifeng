#ifndef PICKTIME_H
#define PICKTIME_H

/*
 * ����ҳHTML��������ȡ����ʱ��
 * ����ֵ��
 *		>=0 - �ɹ������ط���ʱ�������
 *		-1 - ʧ��
 *	������
 *		page - ��ҳ����
 */
extern int get_webpage_time(char *page);
extern int get_webpage_time2(char *page);

extern int pick_a_time(char *text);


/* ��ָ��ʱ��ת��Ϊ����00:00:00������ */
extern int time2day(int t);

/*
 * ��ץȡʱ��Ϊ�ο�������ҳHTML��������ȡ����ʱ��
 * ����ֵ��
 *		����ʱ�������
 *	������
 *		page - ��ҳ����
 *		crawtime - ץȡʱ��
 */
extern int get_pub_time(char *page,int crawTime);

#endif


