#ifndef hashtable_H
#define hashtable_H

// �����Ϣ
typedef struct
{
	void *dat;
}TNodeInfo;

// ���
typedef struct _Node
{
	unsigned key;		// �ڶ���hash�õ��Ĺؼ���
	struct _Node *next;	// ��һ������ָ��

	TNodeInfo info;		// �����Ϣ
}TNode;

#define NODE_NUM_PER_BLK	1000000	// ÿ����������Ľ�����

// ����
typedef struct _NodeBlk
{
	TNode nodes[NODE_NUM_PER_BLK];	// �������
	struct _NodeBlk *next;			// ��һ�������ָ��
}TNodeBlk;

// �ʵ�
typedef struct
{
	TNode **slots;						// ��
	TNodeBlk *startBlk,*endBlk;			// ��ʼ��ͽ�����ָ��
	unsigned slotNum,nodeNum,blkNum;	// ��������������ǰ��Ч�Ľ��������ǰ����
	TNode *wasteNodeStart;				// ���յĽ���������ʼ����ָ��
}TDict;

/*
 * ��ʼ���ʵ�
 * ����ֵ��
 * 		�ɹ����شʵ�ָ�룻ʧ�ܷ���NULL
 * 	������
 * 		slotNum - ������>0���Ƽ�ʹ�ô���������1000003���粻��ʹ����������������
 */
extern TDict* hashtable_init(unsigned slotNum);

/*
 * �ͷŴʵ�
 * ������
 * 		p - �ʵ�ָ��
 */
extern void hashtable_free(TDict *p);

/*
 * ʹ������hash�㷨����64bitǩ�����ۺź͹ؼ��룩
 * ������
 * 		dict - �ʵ�ָ��
 * 		slotNo - 64bitǩ���еĲۺ�
 * 		key - 64bitǩ���еĹؼ���
 * 		keyStr - ��ǩ����ԭʼ��
 */
extern void hashtable_hash(TDict *dict,unsigned *slotNo,unsigned *key,char *keyStr);

/*
 * �ڴʵ��в���һ�����
 * ����ֵ��
 *		1 - ����ɹ�
 *		0 - ��ͬǩ���Ľ���Ѵ���
 *		-1 - ����ʧ��
 * ������
 * 		dict - �ʵ�ָ��
 * 		slotNo - 64bitǩ���еĲۺţ�0 ~ slotNum-1��
 * 		key - 64bitǩ���еĹؼ���
 * 		info - ������Ľ����Ϣ
 */
extern int hashtable_insert(TDict *dict,unsigned slotNo,unsigned key,TNodeInfo *info);

/*
 * �ڴʵ��и���һ�����
 * ����ֵ��
 * 		1 - ���³ɹ�
 * 		0 - û�ҵ��ý�㣬�޷�����
 * 		-1 - ����ʧ��
 * ������
 * 		dict - �ʵ�ָ��
 * 		slotNo - 64bitǩ���еĲۺţ�0 ~ slotNum-1��
 * 		key - 64bitǩ���еĹؼ���
 * 		info - �����µĽ����Ϣ
 */
extern int hashtable_update(TDict *dict,unsigned slotNo,unsigned key,TNodeInfo *info);

/*
 * �ڴʵ���ɾ��һ�����
 * ����ֵ��
 *		1 - ɾ���ɹ�
 *		0 - û�ҵ��ý�㣬�޷�ɾ��
 *		-1 - ɾ��ʧ��
 * ������
 * 		dict - �ʵ�ָ��
 * 		slotNo - 64bitǩ���еĲۺţ�0 ~ slotNum-1��
 * 		key - 64bitǩ���еĹؼ���
 */
extern int hashtable_delete(TDict *dict,unsigned slotNo,unsigned key);

/*
 * �ڴʵ��в���ָ�����
 * ����ֵ��
 * 		1 - �ҵ�
 * 		0 - û�ҵ�
 * ������
 *		dict - �ʵ�ָ��
 * 		slotNo - 64bitǩ���еĲۺţ�0 ~ slotNum-1��
 * 		key - 64bitǩ���еĹؼ���
 * 		info - ����ҵ������ؽ����Ϣ
 */
extern int hashtable_seek(TDict *dict,unsigned slotNo,unsigned key,TNodeInfo *info);

/*
 * ��Ӳ���ļ�����ʵ�
 * ����ֵ��
 * 		�ɹ����شʵ�ָ�룻ʧ�ܷ���NULL
 * ������
 * 		dir - Ŀ¼
 * 		fname - �ļ����Ĺ���ǰ׺����id_dict����ʵ��ļ�Ϊid_dict.conf, id_dict.ind, id_dict.data��
 */
extern TDict* hashtable_load(char *dir,char *fname);

/*
 * ���ʵ�dump��Ӳ���ļ�
 * ����ֵ��
 * 		�ɹ�����0��ʧ�ܷ���-1
 * ������
 * 		dict - �ʵ�ָ��
 * 		dir - Ŀ¼
 * 		fname - �ļ����Ĺ���ǰ׺����id_dict����ʵ��ļ�Ϊid_dict.conf, id_dict.ind, id_dict.data��
 */
extern int hashtable_save(TDict *dict,char *dir,char *fname);

#endif
