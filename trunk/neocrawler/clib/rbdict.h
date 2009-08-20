#ifndef RB_DICT_H
#define RB_DICT_H

struct __RbDict;
typedef struct __RbDict TRbDict;

// 字典KEY比较函数，返回值同strcmp
typedef int             (*DICT_CMP_FUNC)(const void *, const void *);
// 字典元素删除函数，可选
typedef void    (*DICT_DEL_FUNC)(void *);
// 字典元素访问函数，参数分别为key和dat和自定义的args
typedef int             (*DICT_VIS_FUNC)(const void *, void *, void *);

/*
 * The 'key_cmp' function is REQUIRED and may not be NULL. Either 'key_del' or
 * 'dat_del' may be NULL, in which case they will not be called :-).
 */
TRbDict *rb_dict_new(DICT_CMP_FUNC key_cmp, DICT_DEL_FUNC key_del,
                                                  DICT_DEL_FUNC dat_del);
                                                  
/* This function will release all storage used by the dict and, if del is set, it
 * will invoke the user-defined deallocation functions to free the key and / or
 * the data, if applicable.
 */
void rb_dict_destroy(TRbDict *dict, int del);

/*
 * This will attempt to insert the given key and data pair into ``dict''. If
 * the item is already in the dict and ``overwrite'' is non-zero, the new data
 * value will overwrite the old one, and the function will return a positive
 * value. If the node is not present in the dict, it is inserted and the function
 * will return 0. If, however, the function cannot allocate memory to perform the
 * insertion, it will return -1.
 */
int rb_dict_insert(TRbDict *dict, void *key, void *dat, int overwrite);
/*
 * This function will search the given dict for a match for ``key''; if it is
 * found, the data associated with the given key is stored at the location
 * ``dat'', and the function will return 0. Otherwise, the given key and data pair
 * is inserted into the dict and the function will return 1.  ``dat''. However, if
 * it the function was unable to allocate memory it will return -1.
 */
int rb_dict_probe(TRbDict *dict, void *key, void **dat);

/*
 * 如果找到key，则返回0，并且和此key关联的data通过*dat返回
 * 如果找不到key，则返回非零值
 */
int rb_dict_search(TRbDict *dict, const void *key, void **dat);

/*
 * 返回rbdict的一个子集，此子集满足subFlag == 0时，找已key为根的子树
 * subFlag >0 时，为比key大的最小
 */
TRbDict *rb_dict_sub_tree(const void *key, int subFlag);

/*
 * This function will return 0 if the item was found and successfully removed. If
 * the item was not found it will return -1. Any user-defined deallocators will
 * only be invoked if ``del'' is non-zero.
 */
int rb_dict_remove(TRbDict *dict, const void *key, int del);
void rb_dict_empty(TRbDict *dict, int del);

/* Walk the dict
 * Exit when visit return !=0 or walk to the end
 */
void rb_dict_walk(TRbDict *dict, DICT_VIS_FUNC visit, void *args);
unsigned rb_dict_count(const TRbDict *dict);
unsigned rb_dict_height(const TRbDict *dict);
unsigned rb_dict_mheight(const TRbDict *dict);
const void *rb_dict_min(const TRbDict *dict);
const void *rb_dict_max(const TRbDict *dict);


// 迭代器相关
struct __RbIterator;
typedef struct __RbIterator TRbIterator;

TRbIterator *rb_itor_new(TRbDict *dict);
void rb_itor_destroy(TRbIterator *itor);

int rb_itor_valid(const TRbIterator *itor);
void rb_itor_invalidate(TRbIterator *itor);
int rb_itor_next(TRbIterator *itor);
int rb_itor_prev(TRbIterator *itor);
int rb_itor_nextn(TRbIterator *itor, unsigned count);
int rb_itor_prevn(TRbIterator *itor, unsigned count);
int rb_itor_first(TRbIterator *itor);
int rb_itor_last(TRbIterator *itor);
const void *rb_itor_key(const TRbIterator *itor);
void *rb_itor_data(TRbIterator *itor);
const void *rb_itor_cdata(const TRbIterator *itor);
int rb_itor_set_data(TRbIterator *itor, void *dat, int del);
int rb_itor_remove(TRbIterator *itor, int del);

#endif
