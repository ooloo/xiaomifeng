/**
*@file hash_dict.h
*@brief Hash Table declaration
************************************************************************/


#ifndef __DICT_H__
#define __DICT_H__

#include <sys/types.h>

typedef struct __hash_node_t {
	u_int sign1;
	u_int sign2;
	u_int value;
	struct __hash_node_t * next;
} hash_node_t;

/* for dict in mem */
typedef struct{
	int	hash_num;	// number of hash
	int	cur_num;	// number of nodes

	hash_node_t *hash;	// hash

	int	node_avail;	//avail node_num in heap;
	hash_node_t *heap;
} hash_dict_t;



hash_dict_t *hash_create( int hash_num);
int hash_delete(hash_dict_t * hash_dict);

/**@brief insert a node
 *@param hash_dict: a pointer to hash dict
 *@param hash_node: a pointer to hash node
 *@retval -1: fail
 *@retval 1: success
**/
int hash_insert(hash_dict_t* hash_dict, hash_node_t* hash_node);

/**@brief search a node
 *@param hash_dict: a pointer to hash dict
 *@param hash_node: a pointer to hash node for search result
 *@retval 0: not found
 *@retval 1: found
**/
int hash_search(hash_dict_t* hash_dict, hash_node_t* hash_node);

/**@brief create md5 for 64bit hash code
 *@param p: input string 
 *@param sing1: output 0-31bit hash code
 *@param sing2: output 0-63bit hash code
**/
int creat_sign_md64(const char* p,int slen,u_int* sign1,u_int*sign2);

#endif
