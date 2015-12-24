#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include <ctype.h>
#include "hashtable.h"

#define MAX_PATH_LEN	256
#define MAX_LINE_LEN	256

TDict* hashtable_init(unsigned slotNum)
{
	TDict *p;
		
	assert(slotNum>0);

	if(!(p=(TDict*)malloc(sizeof(TDict))))
	{
		return NULL;
	}

	p->slotNum=slotNum;
	//p->slots=(TNode**)calloc(sizeof(TNode*),p->slotNum);
	p->slots=(TNode**)malloc(sizeof(TNode*)*(p->slotNum));
	if(!(p->slots))
	{
		free(p);
		return NULL;
	}
	memset(p->slots,0,sizeof(TNode*)*(p->slotNum));

	p->startBlk=p->endBlk=NULL;
	p->nodeNum=p->blkNum=0;
	p->wasteNodeStart=NULL;
	
	return p;
}

void hashtable_free(TDict *p)
{
	assert(p && p->slots);

	free(p->slots);
	while(p->startBlk)
	{
		p->endBlk=p->startBlk;
		p->startBlk=p->startBlk->next;
		free(p->endBlk);
	}

	free(p);
}

// 一次hash算法
unsigned hash1(char *keyStr,unsigned slotNum)
{
	unsigned long hash = 0;

	assert(keyStr && slotNum>0);

	while (*keyStr)
		hash = ((hash <<7) + *keyStr++) % slotNum;
	
	return hash;
}

// 二次hash算法
unsigned hash2(char *keyStr)
{
	unsigned long h=0;
	unsigned long g;

	assert(keyStr);

	while (*keyStr)
	{
		h = (h<<4) + *keyStr++;
		g=h & 0xF0000000L;
		if (g) h^= g>>24;
		h &=~g;
	}

	return h;
}

void hash(char *keyStr,unsigned slotNum,unsigned *slotNo,unsigned *key)
{
	unsigned signs[4];

	assert(keyStr && slotNum>0 && slotNo && key);
		
	assert(MD5((const unsigned char *)keyStr, strlen(keyStr),(unsigned char *)signs));

	*slotNo = signs[0] % slotNum;
	assert(*slotNo<slotNum);

	*key = signs[1];
}

void hashtable_hash(TDict *dict,unsigned *slotNo,unsigned *key,char *keyStr)
{
	assert(dict && slotNo && key && keyStr);
	
	hash(keyStr,dict->slotNum,slotNo,key);
}

int hashtable_insert(TDict *dict,unsigned slotNo,unsigned key,TNodeInfo *info)
{
	TNode *p,*t;
	TNodeBlk *blk;

	assert(dict && slotNo<dict->slotNum && info);

	p=dict->slots[slotNo];
	while(p)
	{
		if(p->key==key)
			return 0;
		p=p->next;
	}

	t=dict->wasteNodeStart;
	if(t)
		dict->wasteNodeStart=t->next;
	else
	{
		if(dict->nodeNum>=dict->blkNum*NODE_NUM_PER_BLK)
		{
			blk=(TNodeBlk*)malloc(sizeof(TNodeBlk));
			blk->next = NULL;
			if(!blk)
			{
				return -1;
			}

			if(dict->endBlk)
			{
				dict->endBlk->next=blk;
				dict->endBlk=blk;
			}
			else
			{
				assert(!(dict->startBlk));
				dict->startBlk=dict->endBlk=blk;
			}
			dict->blkNum++;

			t=&(dict->endBlk->nodes[0]);
		}
		else
		{
			assert(dict->blkNum>=1 && dict->nodeNum>(dict->blkNum-1)*NODE_NUM_PER_BLK);

			t=&(dict->endBlk->nodes[dict->nodeNum%NODE_NUM_PER_BLK]);
		}
	}
		
	t->key=key;
	memcpy(&(t->info),info,sizeof(TNodeInfo));
	
	t->next=dict->slots[slotNo];
	dict->slots[slotNo]=t;
	
	dict->nodeNum++;
	return 1;	
}

int hashtable_update(TDict *dict,unsigned slotNo,unsigned key,TNodeInfo *info)
{
	TNode *p;
		
	assert(dict && slotNo<dict->slotNum && info);

	p=dict->slots[slotNo];
	while(p)
	{
		if(p->key==key)
		{
			memcpy(&(p->info),info,sizeof(TNodeInfo));
			return 1;
		}
		p=p->next;
	}
	
	return 0;
}

int hashtable_delete(TDict *dict,unsigned slotNo,unsigned key)
{
	TNode *p,*t;

	assert(dict && slotNo<dict->slotNum);

	if(!(p=dict->slots[slotNo]))
		return 0;

	if(p->key==key)
	{
		dict->slots[slotNo]=p->next;

		p->next=dict->wasteNodeStart;
		dict->wasteNodeStart=p;

		dict->nodeNum--;
		return 1;
	}

	while(p->next)
	{
		if(p->next->key==key)
		{
			t=p->next;
			p->next=p->next->next;

			t->next=dict->wasteNodeStart;
			dict->wasteNodeStart=t;

			dict->nodeNum--;
			return 1;
		}
		p=p->next;
	}

	return 0;
}

int hashtable_seek(TDict *dict,unsigned slotNo,unsigned key,TNodeInfo *info)
{
	TNode *p;
		
	assert(dict && slotNo<dict->slotNum && info);

	p=dict->slots[slotNo];
	while(p)
	{
		if(p->key==key)
		{
			memcpy(info,&(p->info),sizeof(TNodeInfo));
			return 1;
		}
		p=p->next;
	}
	
	return 0;
}

/*
int hashtable_seek(TDict *dict,char *keyStr,TNodeInfo *info)
{
	int slotNo;
	unsigned key;
		
	assert(dict && keyStr && info);

	hash(keyStr,dict->slotNum,&slotNo,&key);

	return hashtable_seek(dict,slotNo,key,info);
}
*/

/*
int main(int argc,char **argv)
{
	TDict *dict=hashtable_init(1000003);
	TNodeInfo info;
	char s[20];
	int i;
	
	assert(dict);

	for(i=0;i<100;i++)
	{
		info.a=i;
		sprintf(s,"%d",i);
		assert(hashtable_insert(dict,s,&info)==1);
	}

	for(i=0;i<50;i++)
	{
		info.a=i+10000;
		sprintf(s,"%d",i);
		assert(hashtable_update(dict,s,&info)==1);
	}

	for(i=0;i<80;i++)
	{
		sprintf(s,"%d",i);
		assert(hashtable_delete(dict,s)==1);
	}

	hashtable_free(dict);
	return 0;
}
*/
