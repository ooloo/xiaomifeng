/*
 * hashtable.c
 *
 * Implementation for chained hash table.
 * Copyright (C) 2001 Farooq Mela.
 *
 * $Id: hashtable.c,v 1.2 2001/04/03 08:19:26 farooq Exp farooq $
 */

#include <stdlib.h>
#include <errno.h>

#include "hashtable.h"
#include "dict_private.h"

typedef struct hash_node hash_node;

struct hash_node {
	void		*key;
	void		*dat;
	unsigned	 hash;
	hash_node	*next;
	hash_node	*prev;
};

struct hash_table {
	hash_node		**table;
	unsigned		  size;
	dict_cmp_func	  key_cmp;
	hash_func		  key_hash;
	dict_del_func	  key_del;
	dict_del_func	  dat_del;
	unsigned		  count;
};

struct ht_itor {
	hash_table	*table;
	hash_node	*node;
	int			 slot;
};

hash_table *
ht_new(key_cmp, key_hash, key_del, dat_del, size)
	dict_cmp_func	key_cmp;
	hash_func		key_hash;
	dict_del_func	key_del;
	dict_del_func	dat_del;
	unsigned		size;
{
	hash_table *table;
	unsigned i;

	ASSERT(key_cmp != NULL);
	ASSERT(key_hash != NULL);
	ASSERT(size != 0);

	table = MALLOC(sizeof(*table));
	if (!table)
		return(NULL);
	table->table = MALLOC(size * sizeof(hash_node *));
	if (!table->table) {
		FREE(table);
		errno = ENOMEM;
		return(NULL);
	}
	for (i = 0; i < size; i++)
		table->table[i] = NULL;

	table->size = size;
	table->key_cmp = key_cmp;
	table->key_hash = key_hash;
	table->key_del = key_del;
	table->dat_del = dat_del;
	table->count = 0;

	return(table);
}

dict *
ht_dict_new(key_cmp, key_hash, key_del, dat_del, size)
	dict_cmp_func	key_cmp;
	hash_func		key_hash;
	dict_del_func	key_del;
	dict_del_func	dat_del;
	unsigned		size;
{
	dict *dct;
	hash_table *table;

	ASSERT(key_cmp != NULL);
	ASSERT(key_hash != NULL);
	ASSERT(size != 0);

	dct = MALLOC(sizeof(*dct));
	if (!dct)
		return(NULL);

	table = ht_new(key_cmp, key_hash, key_del, dat_del, size);
	if (!table) {
		FREE(dct);
		errno = ENOMEM;
		return(NULL);
	}

	dct->object = table;
	dct->inew = (inew_func)ht_dict_itor_new;
	dct->destroy = (destroy_func)ht_destroy;
	dct->insert = (insert_func)ht_insert;
	dct->probe = (probe_func)ht_probe;
	dct->search = (search_func)ht_search;
	dct->csearch = (csearch_func)ht_csearch;
	dct->remove = (remove_func)ht_remove;
	dct->empty = (empty_func)ht_empty;
	dct->walk = (walk_func)ht_walk;
	dct->count = (count_func)ht_count;

	return(dct);
}

void
ht_destroy(table, del)
	hash_table	*table;
	int			 del;
{
	ASSERT(table != NULL);

	ht_empty(table, del);
	FREE(table->table);
	FREE(table);
}

int
ht_insert(table, key, dat, overwrite)
	hash_table	*table;
	void		*key;
	void		*dat;
	int			 overwrite;
{
	unsigned hash;
	hash_node *node, *add;

	ASSERT(table != NULL);

	add = MALLOC(sizeof(*add));
	if (!add)
		return(-1);

	add->key = key;
	add->dat = dat;

	hash = (*table->key_hash)(key);
	add->hash = hash;
	hash %= table->size;

	for (node = table->table[hash]; node; node = node->next)
		if ((*table->key_cmp)(key, node->key) == 0) {
			if (overwrite == 0) {
				FREE(add);
				return(1);
			}
			if (table->key_del)
				(*table->key_del)(node->key);
			if (table->dat_del)
				(*table->dat_del)(node->dat);
			return(0);
		}

	add->prev = NULL;
	if (table->table[hash])
		table->table[hash]->prev = add;
	add->next = table->table[hash];
	table->table[hash] = add;
	table->count++;

	return(0);
}

int
ht_probe(table, key, dat)
	hash_table	 *table;
	void		 *key;
	void		**dat;
{
	unsigned hash;
	hash_node *node, *prev, *add;

	ASSERT(table != NULL);
	ASSERT(dat != NULL);

	hash = (*table->key_hash)(key);

	prev = NULL;
	node = table->table[hash % table->size];
	for (; node; prev = node, node = node->next)
		if ((*table->key_cmp)(key, node->key) == 0)
			break;
	if (node) {
		if (prev) { /* Tranpose. */
			SWAP(prev->key, node->key, void *);
			SWAP(prev->dat, node->dat, void *);
			SWAP(prev->hash, node->hash, unsigned);
			node = prev;
		}
		*dat = node->dat;
		return(0);
	}

	add = MALLOC(sizeof(*add));
	if (add == NULL)
		return(-1);
	add->key = key;
	add->dat = dat;
	add->hash = hash;
	hash %= table->size;
	add->prev = NULL;
	add->next = table->table[hash];
	if (table->table[hash])
		table->table[hash]->prev = add;
	table->table[hash] = add;
	table->count++;
	return(1);
}

void *
ht_search(table, key)
	hash_table	*table;
	const void	*key;
{
	hash_node *node, *prev;
	unsigned hash;

	ASSERT(table != NULL);

	hash = (*table->key_hash)(key);
	prev = NULL;
	node = table->table[hash % table->size];
	for (; node; prev = node, node = node->next)
		if (hash == node->hash && (*table->key_cmp)(key, node->key) == 0)
			break;
	if (node) {
		if (prev) {
			/*
			 * Tranpose. This typically offers better performance than move-to-
			 * front, but requires a fairly large number of accesses to
			 * stabilize.
			 */
			SWAP(prev->key, node->key, void *);
			SWAP(prev->dat, node->dat, void *);
			SWAP(prev->hash, node->hash, unsigned);
			node = prev;
		}
		/*
		 * Node was already at front of list.
		 */
		return(node->dat);
	}
	return(NULL);
}

const void *
ht_csearch(table, key)
	const hash_table	*table;
	const void			*key;
{
	ASSERT(table != NULL);

	/*
	 * Cast OK, we want to be able to tranpose, which doesnt modify the
	 * contents of table, only the ordering of items on the chain.
	 */
	return(ht_search((hash_table *)table, key));
}

int
ht_remove(table, key, del)
	hash_table	*table;
	const void	*key;
	int			 del;
{
	hash_node *node, *prev;
	unsigned hash;

	ASSERT(table != NULL);

	hash = (*table->key_hash)(key);
	hash %= table->size;

	prev = NULL;
	for (node = table->table[hash]; node; prev = node, node = node->next)
		if ((*table->key_cmp)(key, node->key) == 0)
			break;
	if (!node)
		return(-1);

	if (prev)
		prev->next = node->next;
	else
		table->table[hash] = node->next;

	if (node->next)
		node->next->prev = prev;

	if (del) {
		if (table->key_del)
			(*table->key_del)(node->key);
		if (table->dat_del)
			(*table->dat_del)(node->dat);
	}
	FREE(node);
	table->count--;
	return(0);
}

void
ht_empty(table, del)
	hash_table	*table;
	int			 del;
{
	hash_node *node, *next;
	unsigned slot;

	ASSERT(table != NULL);

	for (slot = 0; slot < table->size; slot++) {
		for (node = table->table[slot]; node; node = next) {
			next = node->next;
			if (del) {
				if (table->key_del)
					(*table->key_del)(node->key);
				if (table->dat_del)
					(*table->dat_del)(node->dat);
			}
			FREE(node);
		}
		table->table[slot] = NULL;
	}
}

void
ht_walk(table, visit)
	hash_table		*table;
	dict_vis_func	 visit;
{
	hash_node *node;
	unsigned i;

	ASSERT(table != NULL);
	ASSERT(visit != NULL);

	for (i = 0; i < table->size; i++)
		for (node = table->table[i]; node; node = node->next)
			if ((*visit)(node->key, node->dat) == 0) {
				/* Break both loops. */
				i = table->size;
				break;
			}
}

unsigned
ht_count(table)
	const hash_table	*table;
{
	ASSERT(table != NULL);

	return(table->count);
}

unsigned
ht_size(table)
	const hash_table	*table;
{
	ASSERT(table != NULL);

	return(table->size);
}

unsigned
ht_slots_used(table)
	const hash_table	*table;
{
	unsigned i, count = 0;

	ASSERT(table != NULL);

	for (i = 0; i < table->size; i++)
		if (table->table[i])
			count++;
	return(count);
}

int
ht_resize(table, size)
	hash_table	*table;
	unsigned	 size;
{
	struct hash_node **ntable;
	hash_node *node, *next;
	unsigned i, hash;

	ASSERT(table != NULL);
	ASSERT(size != 0);

	if (table->size == size)
		return(0);

	ntable = MALLOC(size * sizeof(hash_node *));
	if (!ntable)
		return(-1);

	for (i = 0; i < size; i++)
		ntable[i] = NULL;

	/*
	 * This way of resizing completely reverses(!) the effects of the trans-
	 * positions that we have been doing in probes and lookups. Hopefully
	 * resizing the hashtable is something that is done rarely or not at all,
	 * so this won't make too much difference.
	 */
	for (i = 0; i < table->size; i++) {
		for (node = table->table[i]; node; node = next) {
			next = node->next;
			hash = node->hash % size;
			node->next = ntable[hash];
			node->prev = NULL;
			if (ntable[hash])
				ntable[hash]->prev = node;
			ntable[hash] = node;
		}
	}

	FREE(table->table);
	table->table = ntable;
	table->size = size;

	return(0);
}

#define RETVALID(itor)	return(itor->node != NULL)

ht_itor *
ht_itor_new(table)
	hash_table *table;
{
	ht_itor *itor;

	ASSERT(table != NULL);

	itor = MALLOC(sizeof(*itor));
	if (!itor)
		return(NULL);

	itor->table = table;
	itor->node = NULL;
	itor->slot = 0;

	ht_itor_first(itor);
	return(itor);
}

dict_itor *
ht_dict_itor_new(table)
	hash_table *table;
{
	dict_itor *itor;

	ASSERT(table != NULL);

	itor = MALLOC(sizeof(*itor));
	if (!itor)
		return(NULL);
	itor->itor = ht_itor_new(table);
	if (!itor->itor) {
		FREE(itor);
		return(NULL);
	}

	itor->destroy = (idestroy_func)ht_itor_destroy;
	itor->valid = (valid_func)ht_itor_valid;
	itor->invalid = (invalidate_func)ht_itor_invalidate;
	itor->next = (next_func)ht_itor_next;
	itor->prev = (prev_func)ht_itor_prev;
	itor->nextn = (nextn_func)ht_itor_nextn;
	itor->prevn = (prevn_func)ht_itor_prevn;
	itor->first = (first_func)ht_itor_first;
	itor->last = (last_func)ht_itor_last;
	itor->key = (key_func)ht_itor_key;
	itor->data = (data_func)ht_itor_data;
	itor->cdata = (cdata_func)ht_itor_cdata;

	return(itor);
}

void
ht_itor_destroy(itor)
	ht_itor *itor;
{
	ASSERT(itor != NULL);

	FREE(itor);
}

int
ht_itor_valid(itor)
	const ht_itor *itor;
{
	ASSERT(itor != NULL);

	RETVALID(itor);
}

void
ht_itor_invalidate(itor)
	ht_itor *itor;
{
	ASSERT(itor != NULL);

	itor->node = NULL;
	itor->slot = 0;
}

int
ht_itor_next(itor)
	ht_itor *itor;
{
	int slot;
	hash_node *node;

	ASSERT(itor != NULL);

	if ((node = itor->node) == NULL)
		return(ht_itor_first(itor));

	slot = itor->slot;
	node = node->next;
	if (node) {
		itor->node = node;
		return(1);
	}

	while (++slot < (int)itor->table->size)
		if ((node = itor->table->table[slot]) != NULL)
			break;
	itor->node = node;
	itor->slot = node ? slot : 0;
	RETVALID(itor);
}

int
ht_itor_prev(itor)
	ht_itor *itor;
{
	int slot;
	hash_node *node;

	ASSERT(itor != NULL);

	if ((node = itor->node) == NULL)
		return(ht_itor_last(itor));

	slot = itor->slot;
	node = node->prev;
	if (node) {
		itor->node = node;
		return(1);
	}

	while (--slot >= 0)
		if ((node = itor->table->table[slot]) != NULL)
			break;
	if (node)
		for (; node->next; node = node->next)
			/* void */;
	itor->node = node;
	itor->slot = slot;

	RETVALID(itor);
}

int
ht_itor_nextn(itor, count)
	ht_itor *itor;
	unsigned count;
{
	ASSERT(itor != NULL);

	if (!count)
		RETVALID(itor);

	while (count) {
		if (!ht_itor_next(itor))
			break;
		count--;
	}
	return(count == 0);
}

int
ht_itor_prevn(itor, count)
	ht_itor *itor;
	unsigned count;
{
	ASSERT(itor != NULL);

	if (!count)
		RETVALID(itor);

	while (count) {
		if (!ht_itor_prev(itor))
			break;
		count--;
	}
	return(count == 0);
}

int
ht_itor_first(itor)
	ht_itor *itor;
{
	unsigned slot;

	ASSERT(itor != NULL);

	for (slot = 0; slot < itor->table->size; slot++)
		if (itor->table->table[slot])
			break;
	if (slot == itor->table->size) {
		itor->node = NULL;
		slot = 0;
	} else {
		itor->node = itor->table->table[slot];
		itor->slot = (int)slot;
	}
	RETVALID(itor);
}

int
ht_itor_last(itor)
	ht_itor *itor;
{
	int slot;

	ASSERT(itor != NULL);

	for (slot = itor->table->size - 1; slot >= 0; slot--)
		if (itor->table->table[slot])
			break;
	if (slot < 0) {
		itor->node = NULL;
		slot = 0;
	} else {
		hash_node *node;

		for (node = itor->table->table[slot]; node->next; node = node->next)
			/* void */;
		itor->node = node;
		itor->slot = slot;
	}
	RETVALID(itor);
}

const void *
ht_itor_key(itor)
	const ht_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->node ? itor->node->key : NULL);
}

void *
ht_itor_data(itor)
	ht_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->node ? itor->node->dat : NULL);
}

const void *
ht_itor_cdata(itor)
	const ht_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->node ? itor->node->dat : NULL);
}
