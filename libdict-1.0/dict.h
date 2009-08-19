/*
 * dict.h
 *
 * Interface for generic access to dictionary library.
 * Copyright (C) 2001 Farooq Mela.
 *
 * $Id: dict.h,v 1.2 2001/04/25 08:52:12 farooq Exp farooq $
 */

#ifndef _DICT_H_
#define _DICT_H_

#ifndef __P
# if defined(__STDC__) || defined(__cplusplus)
#  define __P(x)	x
# else /* !__STDC__ */
#  define __P(x)
# endif
#endif /* !__P */

#ifndef FALSE
#define FALSE	0
#endif

#ifndef TRUE
#define TRUE	!FALSE
#endif

void dict_set_malloc __P((void *(*)(unsigned)));
void dict_set_free __P((void (*)(void *)));

struct dict;
typedef struct dict dict;

typedef int		(*dict_cmp_func) __P((const void *, const void *));
typedef void	(*dict_del_func) __P((void *));
typedef int		(*dict_vis_func) __P((const void *, void *));

void *dict_private __P((dict *dct));
const void *dict_cprivate __P((const dict *dct));
void dict_destroy __P((dict *dct, int del));
int dict_insert __P((dict *dct, void *key, void *dat, int overwrite));
int dict_probe __P((dict *dct, void *key, void **dat));
void *dict_search __P((dict *dct, const void *key));
const void *dict_csearch __P((const dict *dct, const void *key));
int dict_remove __P((dict *dct, const void *key, int del));
void dict_empty __P((dict *dct, int del));
void dict_walk __P((dict *dct, dict_vis_func visit));
unsigned dict_count __P((const dict *dct));

struct dict_itor;
typedef struct dict_itor dict_itor;

dict_itor *dict_itor_new __P((dict *dct));
void dict_itor_destroy __P((dict_itor *itor));

void *dict_itor_private __P((dict_itor *itor));
const void *dict_itor_cprivate __P((const dict_itor *itor));
int dict_itor_valid __P((const dict_itor *itor));
void dict_itor_invalidate __P((dict_itor *itor));
int dict_itor_next __P((dict_itor *itor));
int dict_itor_prev __P((dict_itor *itor));
int dict_itor_nextn __P((dict_itor *itor, unsigned count));
int dict_itor_prevn __P((dict_itor *itor, unsigned count));
int dict_itor_first __P((dict_itor *itor));
int dict_itor_last __P((dict_itor *itor));
const void *dict_itor_key __P((const dict_itor *itor));
void *dict_itor_data __P((dict_itor *itor));
const void *dict_itor_cdata __P((const dict_itor *itor));
int dict_itor_set_data __P((dict_itor *itor, void *dat, int del));
int dict_itor_remove __P((dict_itor *itor, int del));

#include "hashtable.h"
#include "hb_tree.h"
#include "pr_tree.h"
#include "rb_tree.h"
#include "sp_tree.h"
#include "wb_tree.h"

#endif /* !_DICT_H_ */
