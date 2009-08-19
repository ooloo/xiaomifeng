#ifndef hashtable_H
#define hashtable_H

// 结点信息
typedef struct
{
	void *dat;
}TNodeInfo;

// 结点
typedef struct _Node
{
	unsigned key;		// 第二次hash得到的关键码
	struct _Node *next;	// 下一个结点的指针

	TNodeInfo info;		// 结点信息
}TNode;

#define NODE_NUM_PER_BLK	1000000	// 每个结点块包含的结点个数

// 结点块
typedef struct _NodeBlk
{
	TNode nodes[NODE_NUM_PER_BLK];	// 结点数组
	struct _NodeBlk *next;			// 下一个结点块的指针
}TNodeBlk;

// 词典
typedef struct
{
	TNode **slots;						// 槽
	TNodeBlk *startBlk,*endBlk;			// 起始块和结束块指针
	unsigned slotNum,nodeNum,blkNum;	// 参数：槽数、当前有效的结点数、当前块数
	TNode *wasteNodeStart;				// 回收的结点链表的起始结点的指针
}TDict;

/*
 * 初始化词典
 * 返回值：
 * 		成功返回词典指针；失败返回NULL
 * 	参数：
 * 		slotNum - 槽数，>0，推荐使用大素数，如1000003；如不能使用素数则尽量用奇数
 */
extern TDict* hashtable_init(unsigned slotNum);

/*
 * 释放词典
 * 参数：
 * 		p - 词典指针
 */
extern void hashtable_free(TDict *p);

/*
 * 使用两次hash算法计算64bit签名（槽号和关键码）
 * 参数：
 * 		dict - 词典指针
 * 		slotNo - 64bit签名中的槽号
 * 		key - 64bit签名中的关键码
 * 		keyStr - 待签名的原始串
 */
extern void hashtable_hash(TDict *dict,unsigned *slotNo,unsigned *key,char *keyStr);

/*
 * 在词典中插入一个结点
 * 返回值：
 *		1 - 插入成功
 *		0 - 相同签名的结点已存在
 *		-1 - 插入失败
 * 参数：
 * 		dict - 词典指针
 * 		slotNo - 64bit签名中的槽号（0 ~ slotNum-1）
 * 		key - 64bit签名中的关键码
 * 		info - 待插入的结点信息
 */
extern int hashtable_insert(TDict *dict,unsigned slotNo,unsigned key,TNodeInfo *info);

/*
 * 在词典中更新一个结点
 * 返回值：
 * 		1 - 更新成功
 * 		0 - 没找到该结点，无法更新
 * 		-1 - 更新失败
 * 参数：
 * 		dict - 词典指针
 * 		slotNo - 64bit签名中的槽号（0 ~ slotNum-1）
 * 		key - 64bit签名中的关键码
 * 		info - 待更新的结点信息
 */
extern int hashtable_update(TDict *dict,unsigned slotNo,unsigned key,TNodeInfo *info);

/*
 * 在词典中删除一个结点
 * 返回值：
 *		1 - 删除成功
 *		0 - 没找到该结点，无法删除
 *		-1 - 删除失败
 * 参数：
 * 		dict - 词典指针
 * 		slotNo - 64bit签名中的槽号（0 ~ slotNum-1）
 * 		key - 64bit签名中的关键码
 */
extern int hashtable_delete(TDict *dict,unsigned slotNo,unsigned key);

/*
 * 在词典中查找指定结点
 * 返回值：
 * 		1 - 找到
 * 		0 - 没找到
 * 参数：
 *		dict - 词典指针
 * 		slotNo - 64bit签名中的槽号（0 ~ slotNum-1）
 * 		key - 64bit签名中的关键码
 * 		info - 如果找到，返回结点信息
 */
extern int hashtable_seek(TDict *dict,unsigned slotNo,unsigned key,TNodeInfo *info);

/*
 * 从硬盘文件载入词典
 * 返回值：
 * 		成功返回词典指针；失败返回NULL
 * 参数：
 * 		dir - 目录
 * 		fname - 文件名的公共前缀，如id_dict，则词典文件为id_dict.conf, id_dict.ind, id_dict.data等
 */
extern TDict* hashtable_load(char *dir,char *fname);

/*
 * 将词典dump到硬盘文件
 * 返回值：
 * 		成功返回0；失败返回-1
 * 参数：
 * 		dict - 词典指针
 * 		dir - 目录
 * 		fname - 文件名的公共前缀，如id_dict，则词典文件为id_dict.conf, id_dict.ind, id_dict.data等
 */
extern int hashtable_save(TDict *dict,char *dir,char *fname);

#endif
