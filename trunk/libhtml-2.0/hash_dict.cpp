/**
*@file hash_dict.cpp
*@brief  Hash Table implementation
************************************************************************/


#include <wctype.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hash_dict.h"
//#include "charset.h"

#include <openssl/md5.h>
#include <string.h>



int creat_sign_md64(const char* p,int slen,u_int* sign1,u_int*sign2)
{
    unsigned int md5[4];
    assert(p[slen]==0);
    MD5((unsigned char*)p,(unsigned)slen,(unsigned char*)md5);
    *sign1=md5[0]+md5[1];
    *sign2=md5[2]+md5[3];
    return 1;
}

hash_dict_t * hash_create( int hash_num)
{
    hash_dict_t *hash_dict=NULL;

    /* create hash table*/
    if((hash_dict = (hash_dict_t*) malloc(sizeof(hash_dict_t)))==NULL)
        return NULL;
    
    if((hash_dict->hash =(hash_node_t*) calloc(1,hash_num*sizeof(hash_node_t)))==NULL)
    {
        free(hash_dict);
        return NULL;
    }
    
    if((hash_dict->heap = (hash_node_t*) calloc(8,hash_num*sizeof(hash_node_t)))==NULL)
    {
        free(hash_dict->hash);
        free(hash_dict);
        return NULL;
    }   
    hash_dict->hash_num = hash_num;
    hash_dict->cur_num = 0;
    hash_dict->node_avail = 8*hash_num;
    
    return hash_dict;   
}


int hash_delete(hash_dict_t* hash_dict)
{

    	free(hash_dict->heap);

    	free(hash_dict->hash);

    	free(hash_dict);

   	return 1;
}


int hash_search(hash_dict_t * hash_dict, hash_node_t * hash_node)
{
	int hash_entry = (hash_node->sign1+hash_node->sign2)%hash_dict->hash_num;
	hash_node_t *p = &hash_dict->hash[hash_entry];
	
	while(p != NULL)
	{
		if(p->sign1 == hash_node->sign1 && p->sign2 == hash_node->sign2)
		{
			hash_node->value = p->value;
			return 1;
		}
		p = p->next;
	}
	return 0;
}

int hash_insert(hash_dict_t * hash_dict, hash_node_t *hash_node)
{
	int hash_entry = (hash_node->sign1+hash_node->sign2)%hash_dict->hash_num;
	hash_node_t *p = &hash_dict->hash[hash_entry];
	
	if((p->sign1 | p->sign2) == 0)
	{
		p->sign1 = hash_node->sign1;
		p->sign2 = hash_node->sign2;
		p->value = hash_node->value;
		hash_dict->cur_num++;
		return 1;
	}
	
	if(hash_dict->node_avail == 0)
	{
		fprintf(stderr, "Excceed hash size in hash_insert\n");
		return -1;
	}
	
	while(p->next != NULL)
		p = p->next;
	
	hash_dict->node_avail--;
	p->next = &hash_dict->heap[hash_dict->node_avail];
	p = p->next;
	p->sign1 = hash_node->sign1;
	p->sign2 = hash_node->sign2;
	p->value = hash_node->value;
	hash_dict->cur_num++;
	return 1;
}

