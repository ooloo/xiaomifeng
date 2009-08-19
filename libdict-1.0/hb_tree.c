/*
 * hb_tree.c
 *
 * Implementation of height balanced tree.
 * Copyright (C) 2001 Farooq Mela.
 *
 * $Id: hb_tree.c,v 1.1 2001/04/23 01:10:54 farooq Exp farooq $
 */

#include <stdlib.h>
#include <setjmp.h>
#include <errno.h>

#include "hb_tree.h"
#include "dict_private.h"

typedef signed char bal_t;

#define BAL_L	-1
#define BAL_0	 0
#define BAL_R	+1

typedef struct hb_node hb_node;

struct hb_node {
	void	*key;
	void	*dat;
	hb_node	*parent;
	hb_node	*left;
	hb_node	*right;
	bal_t	 bal;
};

struct hb_tree {
	hb_node			*root;
	unsigned		 count;
	dict_cmp_func	 key_cmp;
	dict_del_func	 key_del;
	dict_del_func	 dat_del;
};

struct hb_itor {
	hb_tree	*tree;
	hb_node	*node;
};

static int insert __P((hb_tree *tree, hb_node **node, hb_node *parent,
					   void *key, void *dat, int overwrite, jmp_buf jmp));
static int probe __P((hb_tree *tree, hb_node **node, hb_node *parent,
					  void *key, void **dat, int *add, jmp_buf jmp));
static int delete __P((hb_tree *tree, hb_node **node, const void *key, int del,
					   jmp_buf jmp));
static int fixup __P((hb_tree *tree, hb_node *node));
static int rot_left __P((hb_tree *tree, hb_node *node));
static int rot_right __P((hb_tree *tree, hb_node *node));
static unsigned node_height __P((const hb_node *node));
static unsigned node_mheight __P((const hb_node *node));
static hb_node *node_new __P((void *key, void *dat));
static hb_node *node_min __P((hb_node *node));
static hb_node *node_max __P((hb_node *node));
static hb_node *node_next __P((hb_node *node));
static hb_node *node_prev __P((hb_node *node));

hb_tree *
hb_tree_new(key_cmp, key_del, dat_del)
	dict_cmp_func	key_cmp;
	dict_del_func	key_del;
	dict_del_func	dat_del;
{
	hb_tree *tree;

	ASSERT(key_cmp != NULL);

	tree = MALLOC(sizeof(*tree));
	if (tree == NULL) {
		errno = ENOMEM;
		return(NULL);
	}

	tree->root = NULL;
	tree->count = 0;
	tree->key_cmp = key_cmp;
	tree->key_del = key_del;
	tree->dat_del = dat_del;

	return(tree);
}

dict *
hb_dict_new(key_cmp, key_del, dat_del)
	dict_cmp_func	key_cmp;
	dict_del_func	key_del;
	dict_del_func	dat_del;
{
	dict *dct;
	hb_tree *tree;

	ASSERT(key_cmp != NULL);

	dct = MALLOC(sizeof(*dct));
	if (dct == NULL) {
		errno = ENOMEM;
		return(NULL);
	}

	tree = hb_tree_new(key_cmp, key_del, dat_del);
	if (tree == NULL) {
		FREE(dct);
		errno = ENOMEM;
		return(NULL);
	}

	dct->object = tree;
	dct->inew = (inew_func)hb_dict_itor_new;
	dct->destroy = (destroy_func)hb_tree_destroy;
	dct->insert = (insert_func)hb_tree_insert;
	dct->probe = (probe_func)hb_tree_probe;
	dct->search = (search_func)hb_tree_search;
	dct->csearch = (csearch_func)hb_tree_csearch;
	dct->remove = (remove_func)hb_tree_remove;
	dct->empty = (empty_func)hb_tree_empty;
	dct->walk = (walk_func)hb_tree_walk;
	dct->count = (count_func)hb_tree_count;

	return(dct);
}

void
hb_tree_destroy(tree, del)
	hb_tree	*tree;
	int del;
{
	ASSERT(tree != NULL);

	if (tree->root)
		hb_tree_empty(tree, del);

	FREE(tree);
}

void *
hb_tree_search(tree, key)
	hb_tree		*tree;
	const void	*key;
{
	int rv;
	hb_node *node;

	ASSERT(tree != NULL);

	node = tree->root;
	while (node) {
		rv = (*tree->key_cmp)(key, node->key);
		if (rv == 0)
			break;
		node = rv < 0 ? node->left : node->right;
	}

	return(node ? node->dat : NULL);
}

const void *
hb_tree_csearch(tree, key)
	const hb_tree	*tree;
	const void		*key;
{
	int rv;
	const hb_node *node;

	ASSERT(tree != NULL);

	node = tree->root;
	while (node) {
		rv = (*tree->key_cmp)(key, node->key);
		if (rv == 0)
			break;
		node = rv < 0 ? node->left : node->right;
	}

	return(node ? node->dat : NULL);
}

int
hb_tree_insert(tree, key, dat, overwrite)
	hb_tree		*tree;
	void		*key;
	void		*dat;
	int			 overwrite;
{
	jmp_buf jmp;
	int jv;

	ASSERT(tree != NULL);

	if ((jv = setjmp(jmp)) == 0)
		insert(tree, &tree->root, NULL, key, dat, overwrite, jmp);
	else
		return(jv == 1 ? +1 : -1);
	tree->count++;
	return(0);
}

static int
insert(tree, node, parent, key, dat, overwrite, jmp)
	hb_tree	 *tree;
	hb_node **node;
	hb_node	 *parent;
	void	 *key;
	void	 *dat;
	int		  overwrite;
	jmp_buf	  jmp;
{
	int rv;
	hb_node *n;

	ASSERT(tree != NULL);
	ASSERT(node != NULL);

	if ((n = *node) == NULL) {
		*node = n = node_new(key, dat);
		if (n == NULL)
			longjmp(jmp, 2);
		n->parent = parent;
		return(1);
	}

	rv = (tree->key_cmp)(key, n->key);
	if (rv < 0) {
		rv = -insert(tree, &n->left, n, key, dat, overwrite, jmp);
	} else if (rv > 0) {
		rv = insert(tree, &n->right, n, key, dat, overwrite, jmp);
	} else {
		if (overwrite == 0)
			longjmp(jmp, 1);
		if (tree->key_del)
			(*tree->key_del)(n->key);
		if (tree->dat_del)
			(*tree->dat_del)(n->dat);
		n->key = key;
		n->dat = dat;
		return(0);
	}

	if (rv == 0)
		return(0);

	n->bal += rv;
	return(n->bal ? 1 - fixup(tree, n) : 0);
}

#if 0
int
hb_tree_insert(tree, key, dat, overwrite)
	hb_tree		*tree;
	void		*key;
	void		*dat;
	int			 overwrite;
{
	int rv = 0;
	hb_node *node, *up, *parent = NULL;

	ASSERT(tree != NULL);

	node = tree->root;
	while (node) {
		rv = (*tree->key_cmp)(key, node->key);
		if (rv == 0) {
			if (!overwrite)
				return(1);
			if (tree->key_del)
				(*tree->key_del)(node->key);
			if (tree->dat_del)
				(*tree->dat_del)(node->dat);
			node->key = key;
			node->dat = dat;
			return(0);
		}
		parent = node;
		node = rv < 0 ? node->left : node->right;
	}

	node = node_new(key, dat);
	if (node == NULL)
		return(-1);
	node->parent = parent;
	if (parent == NULL) {
		ASSERT(tree->count == 0);
		tree->root = node;
		tree->count = 1;
		return(0);
	}
	if (rv < 0) {
		parent->left = node;
		rv = -1;
	} else {
		parent->right = node;
		rv = +1;
	}

	while ((up = parent->parent) != NULL) {
		parent->bal += rv;
		rv = 1 - fixup(tree, parent);
		if (!rv)
			break;
		if (up->left == parent)
			rv = -rv;
		node = parent->parent;
		parent = up;
	}
	parent->bal += rv;
	tree->count++;
	return(0);
}
#endif

int
hb_tree_probe(tree, key, dat)
	hb_tree		 *tree;
	void		 *key;
	void		**dat;
{
	jmp_buf jmp;
	int add;

	ASSERT(tree != NULL);
	ASSERT(dat != NULL);

	if (setjmp(jmp) == 0)
		probe(tree, &tree->root, NULL, key, dat, &add, jmp);
	else
		return(-1);
	if (add)
		tree->count++;
	return(add);
}

int
probe(tree, node, parent, key, dat, add, jmp)
	hb_tree		 *tree;
	hb_node		**node;
	hb_node		 *parent;
	void		 *key;
	void		**dat;
	int			 *add;
	jmp_buf		  jmp;
{
	int rv;
	hb_node *n;

	ASSERT(tree != NULL);
	ASSERT(node != NULL);
	ASSERT(dat != NULL);
	ASSERT(add != NULL);

	if ((n = *node) == NULL) {
		*node = n = node_new(key, *dat);
		if (n == NULL)
			longjmp(jmp, 1);
		n->parent = parent;
		*add = 1;
		return(1);
	}

	rv = (*tree->key_cmp)(key, n->key);
	if (rv < 0)
		rv = -probe(tree, &n->left, n, key, dat, add, jmp);
	else if (rv > 0)
		rv = probe(tree, &n->right, n, key, dat, add, jmp);
	else {
		*dat = n->dat;
		*add = 0;
		return(0);
	}

	if (rv == 0)
		return(0);
	n->bal += rv;
	return(n->bal ? 1 - fixup(tree, n) : 0);
}

static int
fixup(tree, node)
	hb_tree *tree;
	hb_node *node;
{
	int needs = 0;

	ASSERT(tree != NULL);
	ASSERT(node != NULL);

	/* Balancing act */
	if (node->bal < BAL_L) {
		ASSERT(node->left != NULL);
		if (node->left->bal <= BAL_0) {		/* RR */
			ASSERT(node->left->left != NULL);
			needs = rot_right(tree, node);
		} else {							/* LR */
			rot_left(tree, node->left);
			rot_right(tree, node);
		}
	} else if (node->bal > BAL_R) {
		ASSERT(node->right != NULL);
		if (node->right->bal >= BAL_0) {	/* LL */
			ASSERT(node->right->right != NULL);
			needs = rot_left(tree, node);
		} else {							/* RL */
			rot_right(tree, node->right);
			rot_left(tree, node);
		}
	} else {
		return(0);
	}
	return(!needs);
}

int
hb_tree_remove(tree, key, del)
	hb_tree		*tree;
	const void	*key;
	int			 del;
{
	int jv;
	jmp_buf jmp;

	if ((jv = setjmp(jmp)) == 0)
		delete(tree, &tree->root, key, del, jmp);
	else
		return(-1);
	tree->count--;
	return(0);
}

static int
delete(tree, node, key, del, jmp)
	hb_tree		 *tree;
	hb_node		**node;
	const void	 *key;
	int			  del;
	jmp_buf		  jmp;
{
	int ret, rv;
	hb_node *n, *out;

	ASSERT(tree != NULL);
	ASSERT(node != NULL);

	if ((n = *node) == NULL)
		longjmp(jmp, 1);

	rv = (*tree->key_cmp)(key, n->key);
	if (rv < 0) {
		ret = -delete(tree, &n->left, key, del, jmp);
	} else if (rv > 0) {
		ret = delete(tree, &n->right, key, del, jmp);
	} else {
		if (n->right && n->left) {
			for (out = n->right; out->left; out = out->left)
				/* void */;
			SWAP(n->key, out->key, void *);
			SWAP(n->dat, out->dat, void *);
			ret = delete(tree, &n->right, key, del, jmp);
		} else if (n->left) {
			out = n->left;
			out->parent = out->parent->parent;
			if (del) {
				if (tree->key_del)
					(*tree->key_del)(n->key);
				if (tree->dat_del)
					(*tree->dat_del)(n->dat);
			}
			FREE(n);
			*node = out;
			return(1);
		} else if (n->right) {
			out = n->right;
			out->parent = out->parent->parent;
			if (del) {
				if (tree->key_del)
					(*tree->key_del)(n->key);
				if (tree->dat_del)
					(*tree->dat_del)(n->dat);
			}
			FREE(n);
			*node = out;
			return(1);
		} else {
			out = n;
			if (del) {
				if (tree->key_del)
					(*tree->key_del)(out->key);
				if (tree->dat_del)
					(*tree->dat_del)(out->dat);
			}
			FREE(out);
			*node = NULL;
			return(1);
		}
	}

	if (ret == 0)
		return(0);
	n->bal -= ret;
	return(n->bal ? fixup(tree, n) : 1);
}

void
hb_tree_empty(tree, del)
	hb_tree *tree;
	int		 del;
{
	hb_node *node, *parent;

	ASSERT(tree != NULL);

	node = tree->root;

	while (node) {
		if (node->left) {
			node = node->left;
			continue;
		}
		if (node->right) {
			node = node->right;
			continue;
		}

		if (del) {
			if (tree->key_del)
				(*tree->key_del)(node->key);
			if (tree->dat_del)
				(*tree->dat_del)(node->dat);
		}

		parent = node->parent;
		FREE(node);
		tree->count--;

		if (parent) {
			if (parent->left == node)
				parent->left = NULL;
			else
				parent->right = NULL;
		}
		node = parent;
	}

	tree->root = NULL;
	ASSERT(tree->count == 0);
}

const void *
hb_tree_min(tree)
	const hb_tree *tree;
{
	const hb_node *node;

	ASSERT(tree != NULL);

	if (tree->root == NULL)
		return(NULL);

	for (node = tree->root; node->left; node = node->left)
		/* void */;
	return(node->key);
}

const void *
hb_tree_max(tree)
	const hb_tree *tree;
{
	const hb_node *node;

	ASSERT(tree != NULL);

	if (tree->root == NULL)
		return(NULL);

	for (node = tree->root; node->right; node = node->left)
		/* void */;
	return(node->key);
}

void
hb_tree_walk(tree, visit)
	hb_tree			*tree;
	dict_vis_func	 visit;
{
	hb_node *node;

	ASSERT(tree != NULL);

	if (tree->root == NULL)
		return;
	for (node = node_min(tree->root); node; node = node_next(node))
		if ((*visit)(node->key, node->dat) == 0)
			break;
}

unsigned
hb_tree_count(tree)
	const hb_tree *tree;
{
	ASSERT(tree != NULL);

	return(tree->count);
}

unsigned
hb_tree_height(tree)
	const hb_tree *tree;
{
	ASSERT(tree != NULL);

	return(tree->root ? node_height(tree->root) : 0);
}

unsigned
hb_tree_mheight(tree)
	const hb_tree *tree;
{
	ASSERT(tree != NULL);

	return(tree->root ? node_mheight(tree->root) : 0);
}

static hb_node *
node_new(key, dat)
	void *key;
	void *dat;
{
	hb_node *node;

	node = MALLOC(sizeof(*node));
	if (node == NULL) {
		errno = ENOMEM;
		return(NULL);
	}

	node->key = key;
	node->dat = dat;
	node->parent = NULL;
	node->left = NULL;
	node->right = NULL;
	node->bal = 0;

	return(node);
}

static hb_node *
node_min(node)
	hb_node *node;
{
	ASSERT(node != NULL);

	while (node->left)
		node = node->left;
	return(node);
}

static hb_node *
node_max(node)
	hb_node *node;
{
	ASSERT(node != NULL);

	while (node->right)
		node = node->right;
	return(node);
}

static hb_node *
node_next(node)
	hb_node		*node;
{
	hb_node *temp;

	ASSERT(node != NULL);

	if (node->right)
		return(node_min(node->right));
	temp = node->parent;
	while (temp && temp->right == node) {
		node = temp;
		temp = temp->parent;
	}
	return(temp);
}

static hb_node *
node_prev(node)
	hb_node		*node;
{
	hb_node *temp;

	ASSERT(node != NULL);

	if (node->left)
		return(node_max(node->left));
	temp = node->parent;
	while (temp && temp->left == node) {
		node = temp;
		temp = temp->parent;
	}
	return(temp);
}

static unsigned
node_height(node)
	const hb_node *node;
{
	unsigned l, r;

	ASSERT(node != NULL);

	l = node->left ? node_height(node->left) + 1 : 0;
	r = node->right ? node_height(node->right) + 1 : 0;
	return(MAX(l, r));
}

static unsigned
node_mheight(node)
	const hb_node *node;
{
	unsigned l, r;

	ASSERT(node != NULL);

	l = node->left ? node_mheight(node->left) + 1 : 0;
	r = node->right ? node_mheight(node->right) + 1 : 0;
	return(MIN(l, r));
}

/*
 * rot_left(T, B):
 *
 *     /             /
 *    B             D
 *   / \           / \
 *  A   D   ==>   B   E
 *     / \       / \
 *    C   E     A   C
 *
 */
static int
rot_left(tree, node)
	hb_tree	*tree;
	hb_node	*node;
{
	int hc;
	hb_node *right;

	ASSERT(tree != NULL);
	ASSERT(node != NULL);
	ASSERT(node->right != NULL);

	right = node->right;
	hc = right->bal ? 0 : 1;
	/*
	 * Update balance information.
	 * Cf. G.H. Gonnet, Handbook of Algorithms and Data Structures
	 */
	node->bal	-= 1 + MAX(right->bal, 0);
	right->bal	-= 1 - MIN(node->bal, 0);
	/*
	 * Perform the rotation.
	 * This is complicated by the use of parent pointers.
	 */
	node->right = right->left;
	if (right->left)
		right->left->parent = node;
	right->parent = node->parent;
	if (node->parent == NULL)
		tree->root = right;
	else {
		if (node->parent->left == node)
			node->parent->left = right;
		else
			node->parent->right = right;
	}
	right->left = node;
	node->parent = right;

	return(hc);
}

/*
 * rot_right(T, D):
 *
 *       /           /
 *      D           B
 *     / \         / \
 *    B   E  ==>  A   D
 *   / \             / \
 *  A   C           C   E
 *
 */
static int
rot_right(tree, node)
	hb_tree	*tree;
	hb_node	*node;
{
	int hc;
	hb_node *left;

	ASSERT(tree != NULL);
	ASSERT(node != NULL);
	ASSERT(node->left != NULL);

	left = node->left;
	hc = left->bal ? 0 : 1;
	node->bal	+= 1 - MIN(left->bal, 0);
	left->bal	+= 1 + MAX(node->bal, 0);
	node->left = left->right;
	if (left->right)
		left->right->parent = node;
	left->parent = node->parent;
	if (node->parent == NULL)
		tree->root = left;
	else {
		if (node->parent->left == node)
			node->parent->left = left;
		else
			node->parent->right = left;
	}
	left->right = node;
	node->parent = left;
	return(hc);
}

hb_itor *
hb_itor_new(tree)
	hb_tree *tree;
{
	hb_itor *itor;

	ASSERT(tree != NULL);

	itor = MALLOC(sizeof(*itor));
	if (itor == NULL) {
		errno = ENOMEM;
		return(NULL);
	}

	itor->tree = tree;
	hb_itor_first(itor);
	return(itor);
}

dict_itor *
hb_dict_itor_new(tree)
	hb_tree *tree;
{
	dict_itor *itor;

	ASSERT(tree != NULL);

	itor = MALLOC(sizeof(*itor));
	if (itor == NULL) {
		errno = ENOMEM;
		return(NULL);
	}

	itor->itor = hb_itor_new(tree);
	if (!itor->itor) {
		FREE(itor);
		return(NULL);
	}

	itor->destroy = (idestroy_func)hb_itor_destroy;
	itor->valid = (valid_func)hb_itor_valid;
	itor->invalid = (invalidate_func)hb_itor_invalidate;
	itor->next = (next_func)hb_itor_next;
	itor->prev = (prev_func)hb_itor_prev;
	itor->nextn = (nextn_func)hb_itor_nextn;
	itor->prevn = (prevn_func)hb_itor_prevn;
	itor->first = (first_func)hb_itor_first;
	itor->last = (last_func)hb_itor_last;
	itor->key = (key_func)hb_itor_key;
	itor->data = (data_func)hb_itor_data;
	itor->cdata = (cdata_func)hb_itor_cdata;

	return(itor);
}

void
hb_itor_destroy(itor)
	hb_itor *itor;
{
	ASSERT(itor != NULL);

	FREE(itor);
}

#define RETVALID(itor)		return(itor->node != NULL)

int
hb_itor_valid(itor)
	const hb_itor *itor;
{
	ASSERT(itor != NULL);

	RETVALID(itor);
}

void
hb_itor_invalidate(itor)
	hb_itor *itor;
{
	ASSERT(itor != NULL);

	itor->node = NULL;
}

int
hb_itor_next(itor)
	hb_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->node == NULL)
		hb_itor_first(itor);
	else
		itor->node = node_next(itor->node);
	RETVALID(itor);
}

int
hb_itor_prev(itor)
	hb_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->node == NULL)
		hb_itor_last(itor);
	else
		itor->node = node_prev(itor->node);
	RETVALID(itor);
}

int
hb_itor_nextn(itor, count)
	hb_itor *itor;
	unsigned count;
{
	ASSERT(itor != NULL);

	if (count) {
		if (itor->node == NULL) {
			hb_itor_first(itor);
			count--;
		}

		while (count-- && itor->node)
			itor->node = node_next(itor->node);
	}

	RETVALID(itor);
}

int
hb_itor_prevn(itor, count)
	hb_itor *itor;
	unsigned count;
{
	ASSERT(itor != NULL);

	if (count) {
		if (itor->node == NULL) {
			hb_itor_last(itor);
			count--;
		}

		while (count-- && itor->node)
			itor->node = node_prev(itor->node);
	}

	RETVALID(itor);
}

int
hb_itor_first(itor)
	hb_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->tree->root == NULL)
		itor->node = NULL;
	else
		itor->node = node_min(itor->tree->root);
	RETVALID(itor);
}

int
hb_itor_last(itor)
	hb_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->tree->root == NULL)
		itor->node = NULL;
	else
		itor->node = node_max(itor->tree->root);
	RETVALID(itor);
}

const void *
hb_itor_key(itor)
	const hb_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->node ? itor->node->key : NULL);
}

void *
hb_itor_data(itor)
	hb_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->node ? itor->node->dat : NULL);
}

const void *
hb_itor_cdata(itor)
	const hb_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->node ? itor->node->dat : NULL);
}
