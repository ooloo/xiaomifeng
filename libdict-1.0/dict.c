/*
 * dict.c
 *
 * Implementation of generic dictionary routines.
 * Copyright (C) 2001 Farooq Mela.
 *
 * $Id: dict.c,v 1.2 2001/04/25 08:49:28 farooq Exp farooq $
 */

#include <stdlib.h>

#include "dict.h"
#include "dict_private.h"

void *(*_dict_malloc)(unsigned) = (void*)malloc;
void (*_dict_free)(void *) = free;

void
dict_set_malloc(malloc_func)
	void *(*malloc_func)(unsigned);
{
	ASSERT(malloc_func != NULL);

	_dict_malloc = malloc_func;
}

void
dict_set_free(free_func)
	void (*free_func)(void *);
{
	ASSERT(malloc_func != NULL);

	_dict_free = free_func;
}

void *
dict_private(dct)
	dict *dct;
{
	ASSERT(dct != NULL);

	return(dct->object);
}

const void *
dict_cprivate(dct)
	const dict *dct;
{
	ASSERT(dct != NULL);

	return(dct->object);
}

void
dict_destroy(dct, del)
	dict	*dct;
	int		 del;
{
	ASSERT(dct != NULL);

	(*dct->destroy)(dct->object, del);
	free(dct);
}

int
dict_insert(dct, key, dat, overwrite)
	dict	*dct;
	void	*key;
	void	*dat;
	int		 overwrite;
{
	ASSERT(dct != NULL);

	return((*dct->insert)(dct->object, key, dat, overwrite));
}

int
dict_probe(dct, key, dat)
	dict	 *dct;
	void	 *key;
	void	**dat;
{
	ASSERT(dct != NULL);

	return((*dct->probe)(dct->object, key, dat));
}

void *
dict_search(dct, key)
	dict		*dct;
	const void	*key;
{
	ASSERT(dct != NULL);

	return((*dct->search)(dct->object, key));
}

const void *
dict_csearch(dct, key)
	const dict	*dct;
	const void	*key;
{
	ASSERT(dct != NULL);

	return((*dct->csearch)(dct->object, key));
}

int
dict_remove(dct, key, del)
	dict		*dct;
	const void	*key;
	int			 del;
{
	ASSERT(dct != NULL);

	return((*dct->remove)(dct->object, key, del));
}

void
dict_empty(dct, del)
	dict	*dct;
	int		 del;
{
	ASSERT(dct != NULL);

	(*dct->empty)(dct->object, del);
}

void
dict_walk(dct, visit)
	dict			*dct;
	dict_vis_func	 visit;
{
	ASSERT(dct != NULL);
	ASSERT(visit != NULL);

	(*dct->walk)(dct->object, visit);
}

unsigned
dict_count(dct)
	const dict *dct;
{
	ASSERT(dct != NULL);

	return((*dct->count)(dct->object));
}

dict_itor *
dict_itor_new(dct)
	dict *dct;
{
	ASSERT(dct != NULL);

	return((*dct->inew)(dct->object));
}

void
dict_itor_destroy(itor)
	dict_itor *itor;
{
	ASSERT(itor != NULL);

	(*itor->destroy)(itor->itor);
	free(itor);
}

void *
dict_itor_private(itor)
	dict_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->itor);
}

const void *
dict_itor_cprivate(itor)
	const dict_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->itor);
}

int
dict_itor_valid(itor)
	const dict_itor *itor;
{
	ASSERT(itor != NULL);

	return((*itor->valid)(itor->itor));
}

void
dict_itor_invalidate(itor)
	dict_itor *itor;
{
	ASSERT(itor != NULL);

	((*itor->invalid)(itor->itor));
}

int
dict_itor_next(itor)
	dict_itor *itor;
{
	ASSERT(itor != NULL);

	return((*itor->next)(itor->itor));
}

int
dict_itor_prev(itor)
	dict_itor *itor;
{
	ASSERT(itor != NULL);

	return((*itor->prev)(itor->itor));
}

int
dict_itor_nextn(itor, count)
	dict_itor	*itor;
	unsigned	 count;
{
	ASSERT(itor != NULL);

	return((*itor->nextn)(itor->itor, count));
}

int
dict_itor_prevn(itor, count)
	dict_itor	*itor;
	unsigned	 count;
{
	ASSERT(itor != NULL);

	return((*itor->prevn)(itor->itor, count));
}

int
dict_itor_first(itor)
	dict_itor *itor;
{
	ASSERT(itor != NULL);

	return((*itor->first)(itor->itor));
}

int
dict_itor_last(itor)
	dict_itor *itor;
{
	ASSERT(itor != NULL);

	return((*itor->last)(itor->itor));
}

const void *
dict_itor_key(itor)
	const dict_itor *itor;
{
	ASSERT(itor != NULL);

	return((*itor->key)(itor->itor));
}

void *
dict_itor_data(itor)
	dict_itor *itor;
{
	ASSERT(itor != NULL);

	return((*itor->data)(itor->itor));
}

const void *
dict_itor_cdata(itor)
	const dict_itor *itor;
{
	ASSERT(itor != NULL);

	return((*itor->cdata)(itor->itor));
}

int
dict_itor_set_data(itor, dat, del)
	dict_itor	*itor;
	void		*dat;
	int			 del;
{
	ASSERT(itor != NULL);

	return((*itor->dataset)(itor, dat, del));
}

int
dict_itor_remove(itor, del)
	dict_itor	*itor;
	int			 del;
{
	ASSERT(itor != NULL);

	return((*itor->remove)(itor, del));
}
