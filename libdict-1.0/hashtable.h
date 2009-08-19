/*
 * hashtable.h
 *
 * Interface for chained hash table.
 * Copyright (C) 2001 Farooq Mela.
 *
 * $Id: hashtable.h,v 1.2 2001/04/03 08:20:32 farooq Exp farooq $
 */

#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include "dict.h"

struct hash_table;
typedef struct hash_table hash_table;

typedef unsigned (*hash_func) __P((const void *));

hash_table *ht_new __P((dict_cmp_func key_cmp, hash_func key_hash,
						dict_del_func key_del, dict_del_func dat_del,
						unsigned size));
dict *ht_dict_new __P((dict_cmp_func key_cmp, hash_func key_hash,
					   dict_del_func key_del, dict_del_func dat_del,
					   unsigned size));
void ht_destroy __P((hash_table *table, int del));

int ht_insert __P((hash_table *table, void *key, void *dat, int overwrite));
int ht_probe __P((hash_table *table, void *key, void **dat));
void *ht_search __P((hash_table *table, const void *key));
const void *ht_csearch __P((const hash_table *table, const void *key));
int ht_remove __P((hash_table *table, const void *key, int del));
void ht_empty __P((hash_table *table, int del));
void ht_walk __P((hash_table *table, dict_vis_func visit));
unsigned ht_count __P((const hash_table *table));
unsigned ht_size __P((const hash_table *table));
unsigned ht_slots_used __P((const hash_table *table));
int ht_resize __P((hash_table *table, unsigned size));

struct ht_itor;
typedef struct ht_itor ht_itor;

ht_itor *ht_itor_new __P((hash_table *table));
dict_itor *ht_dict_itor_new __P((hash_table *table));
void ht_itor_destroy __P((ht_itor *));

int ht_itor_valid __P((const ht_itor *itor));
void ht_itor_invalidate __P((ht_itor *itor));
int ht_itor_next __P((ht_itor *itor));
int ht_itor_prev __P((ht_itor *itor));
int ht_itor_nextn __P((ht_itor *itor, unsigned count));
int ht_itor_prevn __P((ht_itor *itor, unsigned count));
int ht_itor_first __P((ht_itor *itor));
int ht_itor_last __P((ht_itor *itor));
const void *ht_itor_key __P((const ht_itor *itor));
void *ht_itor_data __P((ht_itor *itor));
const void *ht_itor_cdata __P((const ht_itor *itor));
int ht_itor_set_data __P((ht_itor *itor, void *dat, int del));
int ht_itor_remove __P((ht_itor *itor, int del));

#endif /* !_HASHTABLE_H_ */
