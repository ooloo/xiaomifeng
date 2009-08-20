#include <assert.h>
#include <stdlib.h>


#include "block_manager.h"

/*
 * create block heap
 */ 
block_heap_t *block_manager::block_heap_create(int node_size, int unit_num)
{
	block_heap_t *block_heap;
	block_t *block;
	
	assert(node_size > 0);
	assert(unit_num > 0);

	block = NULL;
	block_heap = NULL;
	
	//struct
	block_heap = (block_heap_t *)calloc(sizeof(block_heap_t), 1);
	if (block_heap == NULL) {
		goto FAILED;
	}
	
	//block
	block = (block_t *)calloc(sizeof(block_t), 1);
	if (block == NULL) {
		goto FAILED;
	}
	block->p = (char *)malloc(node_size * unit_num);
	if (block->p == NULL) {
		goto FAILED;
	}
	block->next = NULL;
	
	//others
	block_heap->node_size = node_size;
	block_heap->unit_num = unit_num;
	block_heap->block = block;
	block = NULL;
	block_heap->available = block_heap->block->p;

	return block_heap;

FAILED:
	if (block_heap != NULL) {
		block_heap_delete(block_heap);
	}
	if (block != NULL) {
		if (block->p != NULL) {
			free(block->p);
		}
		free(block);
	}
	return NULL;
}

/*
 * allocate one node
 */ 
void *block_manager::block_heap_allocate(block_heap_t *block_heap)
{
	void *p;
	block_t *block;
	
	p = block_heap->available;

	block_heap->available += block_heap->node_size;
	if ( block_heap->available == 
			block_heap->block->p + block_heap->node_size * block_heap->unit_num) {
		block = (block_t *)calloc(sizeof(block_t), 1);
		if (block == NULL) {
			goto FAILED;
		}
		block->p = (char *)malloc(block_heap->node_size * block_heap->unit_num);
		if (block->p == NULL) {
			goto FAILED;
		}
		block->next = block_heap->block;
		
		block_heap->block = block;
		block_heap->available = block_heap->block->p;
	}
	assert(block_heap->available < block_heap->block->p+block_heap->node_size*block_heap->unit_num);
	return p;
FAILED:
	if (block != NULL) {
		if (block->p != NULL) {
			free(block->p);
		}
		free(block);
	}
	return NULL;
}

/*
 * test if it is a clean heap
 */ 
int block_manager::is_clean_heap(block_heap_t *block_heap)
{
	if (block_heap->block->next != NULL) {
		return 0;
	}
	if (block_heap->available != block_heap->block->p) {
		return 0;
	}
	return 1;
}

/*
 * renew block heap, free blocks that added in block_heap_allocate
 */ 
void block_manager::block_heap_renew(block_heap_t *block_heap)
{
	block_t *block, *p;
	
	block = block_heap->block->next; 
	while (block != NULL) {
		p = block;
		block = block->next;
		free(p->p);
		free(p);
	}

	block_heap->block->next = NULL;
	block_heap->available = block_heap->block->p;
}

/*
 * delete all block heap
 */ 
void block_manager::block_heap_delete(block_heap_t *block_heap)
{
	block_t *block, *p;

	block = block_heap->block;
	while (block != NULL) {
		p = block;
		block = block->next;
		free(p->p);
		free(p);
	}
	free(block_heap);
}
