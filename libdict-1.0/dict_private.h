/*
 * dict_private.h
 *
 * Private definitions for libdict.
 * Copyright (C) 2001 Farooq Mela.
 *
 * $Id: dict_private.h,v 1.1 2001/04/23 01:11:50 farooq Exp farooq $
 */

#ifndef _DICT_PRIVATE_H_
#define _DICT_PRIVATE_H_

#include "dict.h"

typedef dict_itor	*(*inew_func)	__P((void *obj));
typedef void		 (*destroy_func)__P((void *obj, int del));
typedef int			 (*insert_func)	__P((void *obj, void *k, void *d, int ow));
typedef int			 (*probe_func)	__P((void *obj, void *key, void **dat));
typedef void		*(*search_func)	__P((void *obj, const void *key));
typedef const void	*(*csearch_func)__P((const void *obj, const void *key));
typedef int			 (*remove_func)	__P((void *obj, const void *key, int del));
typedef void		 (*empty_func)	__P((void *obj, int del));
typedef void		 (*walk_func)	__P((void *obj, dict_vis_func visit));
typedef unsigned	 (*count_func)	__P((const void *obj));

struct dict {
	void			*object;
	inew_func		 inew;
	destroy_func	 destroy;
	insert_func		 insert;
	probe_func		 probe;
	search_func		 search;
	csearch_func	 csearch;
	remove_func		 remove;
	empty_func		 empty;
	walk_func		 walk;
	count_func		 count;
};

typedef void		 (*idestroy_func)	__P((void *itor));
typedef int			 (*valid_func)		__P((const void *itor));
typedef void		 (*invalidate_func)	__P((void *itor));
typedef int			 (*next_func)		__P((void *itor));
typedef int			 (*prev_func)		__P((void *itor));
typedef int			 (*nextn_func)		__P((void *itor, unsigned count));
typedef int			 (*prevn_func)		__P((void *itor, unsigned count));
typedef int			 (*first_func)		__P((void *itor));
typedef int			 (*last_func)		__P((void *itor));
typedef void		*(*key_func)		__P((void *itor));
typedef void		*(*data_func)		__P((void *itor));
typedef const void	*(*cdata_func)		__P((const void *itor));
typedef int			 (*dataset_func)	__P((void *itor, void *dat, int del));
typedef int			 (*iremove_func)	__P((void *itor, int del));
typedef int			 (*compare_func)	__P((void *itor1, void *itor2));

struct dict_itor {
	void			*itor;
	idestroy_func	 destroy;
	valid_func		 valid;
	invalidate_func	 invalid;
	next_func		 next;
	prev_func		 prev;
	nextn_func		 nextn;
	prevn_func		 prevn;
	first_func		 first;
	last_func		 last;
	key_func		 key;
	data_func		 data;
	cdata_func		 cdata;
	dataset_func	 dataset;
	iremove_func	 remove;
	compare_func	 compare;
};

#ifndef NDEBUG
# include <stdio.h>
# undef ASSERT
# if defined(__GNUC__)
#  define ASSERT(expr)														\
	if (!(expr))															\
		fprintf(stderr, "%s:%d (%s) assertion failed: `%s'\n",				\
				__FILE__, __LINE__, __PRETTY_FUNCTION__, #expr),			\
		abort();
# else
#  define ASSERT(expr)														\
	if (!(expr))															\
		fprintf(stderr, "%s:%d assertion failed: `%s'\n",					\
				__FILE__, __LINE__, #expr),									\
		abort();
# endif
#else
# define ASSERT(expr)
#endif

extern void *(*_dict_malloc)(unsigned);
extern void (*_dict_free)(void *);

#define MIN(a,b)	((a) < (b) ? (a) : (b))
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#define SWAP(a,b,t)	{ t _t = (a); (a) = (b); (b) = _t; }
#define MALLOC(n)	(*_dict_malloc)(n)
#define FREE(p)		(*_dict_free)(p)

#endif /* !_DICT_PRIVATE_H_ */
