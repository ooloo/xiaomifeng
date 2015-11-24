#ifndef __BLOCK_HEAP_H__
#define __BLOCK_HEAP_H__

typedef struct _block_t {
    char *p;
    struct _block_t *next;
} block_t;

typedef struct _block_heap_t {
    int node_size;
    int unit_num;
    char *available;
    block_t *block;
} block_heap_t;


class block_manager {
  public:
    static block_heap_t *block_heap_create(int node_size, int unit_num);

    static void *block_heap_allocate(block_heap_t * block_heap);

    static int is_clean_heap(block_heap_t * block_heap);

    static void block_heap_renew(block_heap_t * block_heap);

    static void block_heap_delete(block_heap_t * block_heap);

};
#endif
