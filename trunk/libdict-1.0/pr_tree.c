/*
 * pr_tree.c
 *
 * Implementation of path reduction tree.
 * Copyright (C) 2001 Farooq Mela.
 *
 * $Id: pr_tree.c,v 1.1 2001/04/23 01:12:05 farooq Exp farooq $
 */

#include <stdlib.h>
#include <setjmp.h>
#include <errno.h>

#include "pr_tree.h"
#include "dict_private.h"

typedef unsigned long wt_t;

typedef struct pr_node pr_node;
struct pr_node {
	void	*key;
	void	*dat;
	pr_node	*parent;
	pr_node	*left;
	pr_node	*right;
	wt_t	 weight;
};

#define WEIGHT(n)	((n) ? (n)->weight : 1)

struct pr_tree {
	pr_node			*root;
	unsigned		 count;
	dict_cmp_func	 key_cmp;
	dict_del_func	 key_del;
	dict_del_func	 dat_del;
};

struct pr_itor {
	pr_tree	*tree;
	pr_node	*node;
};

static void fixup __P((pr_tree *tree, pr_node *node));
static int delete __P((pr_tree *tree, pr_node **node, const void *key,
					   int del));
static void rot_left __P((pr_tree *tree, pr_node *node));
static void rot_right __P((pr_tree *tree, pr_node *node));
static unsigned node_height __P((const pr_node *node));
static unsigned node_mheight __P((const pr_node *node));
static pr_node *node_new __P((void *key, void *dat));
static pr_node *node_min __P((pr_node *node));
static pr_node *node_max __P((pr_node *node));
static pr_node *node_next __P((pr_node *node));
static pr_node *node_prev __P((pr_node *node));

pr_tree *
pr_tree_new(key_cmp, key_del, dat_del)
	dict_cmp_func	key_cmp;
	dict_del_func	key_del;
	dict_del_func	dat_del;
{
	pr_tree *tree;

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
pr_dict_new(key_cmp, key_del, dat_del)
	dict_cmp_func	key_cmp;
	dict_del_func	key_del;
	dict_del_func	dat_del;
{
	dict *dct;
	pr_tree *tree;

	ASSERT(key_cmp != NULL);

	dct = MALLOC(sizeof(*dct));
	if (dct == NULL) {
		errno = ENOMEM;
		return(NULL);
	}

	tree = pr_tree_new(key_cmp, key_del, dat_del);
	if (tree == NULL) {
		FREE(dct);
		errno = ENOMEM;
		return(NULL);
	}

	dct->object = tree;
	dct->inew = (inew_func)pr_dict_itor_new;
	dct->destroy = (destroy_func)pr_tree_destroy;
	dct->insert = (insert_func)pr_tree_insert;
	dct->probe = (probe_func)pr_tree_probe;
	dct->search = (search_func)pr_tree_search;
	dct->csearch = (csearch_func)pr_tree_csearch;
	dct->remove = (remove_func)pr_tree_remove;
	dct->empty = (empty_func)pr_tree_empty;
	dct->walk = (walk_func)pr_tree_walk;
	dct->count = (count_func)pr_tree_count;

	return(dct);
}

void
pr_tree_destroy(tree, del)
	pr_tree	*tree;
	int del;
{
	ASSERT(tree != NULL);

	if (tree->root)
		pr_tree_empty(tree, del);

	FREE(tree);
}

void *
pr_tree_search(tree, key)
	pr_tree		*tree;
	const void	*key;
{
	int rv;
	pr_node *node;

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
pr_tree_csearch(tree, key)
	const pr_tree	*tree;
	const void		*key;
{
	int rv;
	const pr_node *node;

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

static void
fixup(tree, node)
	pr_tree		*tree;
	pr_node		*node;
{
	pr_node *temp;
	wt_t wl, wr;

	ASSERT(tree != NULL);
	ASSERT(node != NULL);

	/*
	 * Path-reduction trees are similar to weight-balanced trees, except that
	 * rotations are only made when they can reduce the total internal path
	 * length of the tree.
	 *
	 * Consider a node N.
	 * A single left rotation is performed when
	 * WEIGHT(LEFT(N)) < WEIGHT(RIGHT(RIGHT(N))
	 * A double left rotation is performed when
	 * WEIGHT(LEFT(N)) < WEIGHT(LEFT(RIGHT(N))
	 *
	 * A single right rotation is performed when
	 * WEIGHT(RIGHT(N)) > WEIGHT(LEFT(LEFT(n))
	 * A double right rotation is performed when
	 * WEIGHT(RIGHT(N)) > WEIGHT(RIGHT(LEFT(N))
	 *
	 * Although the worst case number of rotations for a single insertion or
	 * deletion is O(n), the amortized worst-case number of rotations is
	 * .44042lg(n) + O(1) for insertion, and .42062lg(n) + O(1) for deletion.
	 */
	wl = WEIGHT(node->left);
	wr = WEIGHT(node->right);
	if (wr > wl) {
		if (WEIGHT(node->right->right) > wl) {			/* LL */
			temp = node->right;
			rot_left(tree, node);
			if (temp->left)
				fixup(tree, temp->left);
		} else if (WEIGHT(node->right->left) > wl) {	/* RL */
			temp = node->right;
			rot_right(tree, node->right);
			rot_left(tree, node);
			if (temp->left)
				fixup(tree, temp->left);
			if (temp->right)
				fixup(tree, temp->right);
		}
	} else if (wl > wr) {
		if (WEIGHT(node->left->left) > wr) {			/* RR */
			temp = node->left;
			rot_right(tree, node);
			if (temp->right)
				fixup(tree, temp->right);
		} else if (WEIGHT(node->left->right) > wr) {	/* LR */
			temp = node->left;
			rot_left(tree, node->left);
			rot_right(tree, node);
			if (temp->left)
				fixup(tree, temp->left);
			if (temp->right)
				fixup(tree, temp->right);
		}
	}
}

/*
 * Macro expansion of above function. Because path reduction is recursive, and
 * we cannot have a macro "call" itself, we use the actual fixup() function.
 *
 * This macro assumes a variable "tree" which exists in the scope from which
 * it is expanded, which is a pointer to the tree of which n is a node.
 */
#define FIXUP(n)															\
{																			\
	pr_node *temp;															\
	wt_t wl, wr;															\
																			\
	wl = WEIGHT((n)->left);													\
	wr = WEIGHT((n)->right);												\
	if (wr > wl) {															\
		if (WEIGHT((n)->right->right) > wl) {			/* LL */			\
			temp = (n)->right;												\
			rot_left(tree, (n));											\
			if (temp->left)													\
				fixup(tree, temp->left);									\
		} else if (WEIGHT((n)->right->left) > wl) {		/* RL */			\
			temp = (n)->right;												\
			rot_right(tree, (n)->right);									\
			rot_left(tree, (n));											\
			if (temp->left)													\
				fixup(tree, temp->left);									\
			if (temp->right)												\
				fixup(tree, temp->right);									\
		}																	\
	} else if (wr < wl) {													\
		if (WEIGHT(node->left->left) > wr) {			/* RR */			\
			temp = node->left;												\
			rot_right(tree, node);											\
			if (temp->right)												\
				fixup(tree, temp->right);									\
		} else if (WEIGHT(node->left->right) > wr) {	/* LR */			\
			temp = node->left;												\
			rot_left(tree, node->left);										\
			rot_right(tree, node);											\
			if (temp->left)													\
				fixup(tree, temp->left);									\
			if (temp->right)												\
				fixup(tree, temp->right);									\
		}																	\
	}																		\
}

int
pr_tree_insert(tree, key, dat, overwrite)
	pr_tree		*tree;
	void		*key;
	void		*dat;
	int			 overwrite;
{
	int rv = 0;
	pr_node *node, *parent = NULL;

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
	if (parent) {
		if (rv < 0)
			parent->left = node;
		else
			parent->right = node;
	} else {
		ASSERT(tree->count == 0);
		tree->root = node;
		tree->count = 1;
		return(0);
	}

	while ((node = parent) != NULL) {
		parent = parent->parent;
	/*	if (WEIGHT(node->left) != WEIGHT(node->right))
			fixup(tree, node); */
		FIXUP(node);
	}

	tree->count++;
	return(0);
}

int
pr_tree_probe(tree, key, dat)
	pr_tree		 *tree;
	void		 *key;
	void		**dat;
{
	int rv = 0;
	pr_node *node, *parent = NULL;

	ASSERT(tree != NULL);

	node = tree->root;
	while (node) {
		rv = (*tree->key_cmp)(key, node->key);
		if (rv == 0) {
			*dat = node->dat;
			return(0);
		}
		parent = node;
		node = rv < 0 ? node->left : node->right;
	}

	node = node_new(key, dat);
	if (node == NULL)
		return(-1);
	node->parent = parent;
	if (parent) {
		if (rv < 0)
			parent->left = node;
		else
			parent->right = node;
	} else {
		ASSERT(tree->count == 0);
		tree->root = node;
		tree->count = 1;
		return(1);
	}

	while ((node = parent) != NULL) {
		parent = parent->parent;
	/*	if (WEIGHT(node->left) != WEIGHT(node->right))
			fixup(tree, node); */
		FIXUP(node);
	}

	tree->count++;
	return(1);
}

int
pr_tree_remove(tree, key, del)
	pr_tree		*tree;
	const void	*key;
	int			 del;
{
	int rv;

	rv = delete(tree, &tree->root, key, del);
	if (rv == 0)
		tree->count--;
	return(0);
}

static int
delete(tree, node, key, del)
	pr_tree		 *tree;
	pr_node		**node;
	const void	 *key;
	int			  del;
{
	int rv;
	pr_node *out;

	ASSERT(tree != NULL);
	ASSERT(node != NULL);

	if (*node == NULL)
		return(-1);

	out = *node;
	rv = (*tree->key_cmp)(key, out->key);
	if (rv < 0) {
		if ((rv = delete(tree, &out->left, key, del)) != 0)
			return(rv);
	} else if (rv > 0) {
		if ((rv = delete(tree, &out->right, key, del)) != 0)
			return(rv);
	} else if (out->left == NULL) {
		out = out->right;
		if (out)
			out->parent = (*node)->parent;
		(*node) = out;
	} else if (out->right == NULL) {
		out = out->left;
		if (out)
			out->parent = (*node)->parent;
		(*node) = out;
	} else if (WEIGHT(out->left) > WEIGHT(out->right)) {
		if (WEIGHT(out->left->left) < WEIGHT(out->left->right))
			rot_left(tree, out->left);
		out = out->left;
		rot_right(tree, *node);
		if ((rv = delete(tree, &out->right, key, del)) != 0)
			return(rv);
	} else {
		if (WEIGHT(out->right->left) > WEIGHT(out->right->right))
			rot_right(tree, out->right);
		out = out->right;
		rot_left(tree, *node);
		if ((rv = delete(tree, &out->left, key, del)) != 0)
			return(rv);
	}

	out = *node;
	if (out)
		out->weight = WEIGHT(out->left) + WEIGHT(out->right);
	return(0);
}

void
pr_tree_empty(tree, del)
	pr_tree *tree;
	int		 del;
{
	pr_node *node, *parent;

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
pr_tree_min(tree)
	const pr_tree *tree;
{
	const pr_node *node;

	ASSERT(tree != NULL);

	if (tree->root == NULL)
		return(NULL);

	for (node = tree->root; node->left; node = node->left)
		/* void */;
	return(node->key);
}

const void *
pr_tree_max(tree)
	const pr_tree *tree;
{
	const pr_node *node;

	ASSERT(tree != NULL);

	if (tree->root == NULL)
		return(NULL);

	for (node = tree->root; node->right; node = node->left)
		/* void */;
	return(node->key);
}

void
pr_tree_walk(tree, visit)
	pr_tree			*tree;
	dict_vis_func	 visit;
{
	pr_node *node;

	ASSERT(tree != NULL);

	if (tree->root == NULL)
		return;
	for (node = node_min(tree->root); node; node = node_next(node))
		if ((*visit)(node->key, node->dat) == 0)
			break;
}

unsigned
pr_tree_count(tree)
	const pr_tree *tree;
{
	ASSERT(tree != NULL);

	return(tree->count);
}

unsigned
pr_tree_height(tree)
	const pr_tree *tree;
{
	ASSERT(tree != NULL);

	return(tree->root ? node_height(tree->root) : 0);
}

unsigned
pr_tree_mheight(tree)
	const pr_tree *tree;
{
	ASSERT(tree != NULL);

	return(tree->root ? node_mheight(tree->root) : 0);
}

static pr_node *
node_new(key, dat)
	void *key;
	void *dat;
{
	pr_node *node;

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
	node->weight = 2;

	return(node);
}

static pr_node *
node_min(node)
	pr_node *node;
{
	ASSERT(node != NULL);

	while (node->left)
		node = node->left;
	return(node);
}

static pr_node *
node_max(node)
	pr_node *node;
{
	ASSERT(node != NULL);

	while (node->right)
		node = node->right;
	return(node);
}

static pr_node *
node_next(node)
	pr_node		*node;
{
	pr_node *temp;

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

static pr_node *
node_prev(node)
	pr_node		*node;
{
	pr_node *temp;

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
	const pr_node *node;
{
	unsigned l, r;

	ASSERT(node != NULL);

	l = node->left ? node_height(node->left) + 1 : 0;
	r = node->right ? node_height(node->right) + 1 : 0;
	return(MAX(l, r));
}

static unsigned
node_mheight(node)
	const pr_node *node;
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
static void
rot_left(tree, node)
	pr_tree	*tree;
	pr_node	*node;
{
	pr_node *right;

	ASSERT(tree != NULL);
	ASSERT(node != NULL);
	ASSERT(node->right != NULL);

	right = node->right;
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

	node->weight = WEIGHT(node->left) + WEIGHT(node->right);
	right->weight = WEIGHT(right->left) + WEIGHT(right->right);
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
static void
rot_right(tree, node)
	pr_tree	*tree;
	pr_node	*node;
{
	pr_node *left;

	ASSERT(tree != NULL);
	ASSERT(node != NULL);
	ASSERT(node->left != NULL);

	left = node->left;
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

	node->weight = WEIGHT(node->left) + WEIGHT(node->right);
	left->weight = WEIGHT(left->left) + WEIGHT(left->right);
}

pr_itor *
pr_itor_new(tree)
	pr_tree *tree;
{
	pr_itor *itor;

	ASSERT(tree != NULL);

	itor = MALLOC(sizeof(*itor));
	if (itor == NULL) {
		errno = ENOMEM;
		return(NULL);
	}

	itor->tree = tree;
	pr_itor_first(itor);
	return(itor);
}

dict_itor *
pr_dict_itor_new(tree)
	pr_tree *tree;
{
	dict_itor *itor;

	ASSERT(tree != NULL);

	itor = MALLOC(sizeof(*itor));
	if (itor == NULL) {
		errno = ENOMEM;
		return(NULL);
	}

	itor->itor = pr_itor_new(tree);
	if (!itor->itor) {
		FREE(itor);
		return(NULL);
	}

	itor->destroy = (idestroy_func)pr_itor_destroy;
	itor->valid = (valid_func)pr_itor_valid;
	itor->invalid = (invalidate_func)pr_itor_invalidate;
	itor->next = (next_func)pr_itor_next;
	itor->prev = (prev_func)pr_itor_prev;
	itor->nextn = (nextn_func)pr_itor_nextn;
	itor->prevn = (prevn_func)pr_itor_prevn;
	itor->first = (first_func)pr_itor_first;
	itor->last = (last_func)pr_itor_last;
	itor->key = (key_func)pr_itor_key;
	itor->data = (data_func)pr_itor_data;
	itor->cdata = (cdata_func)pr_itor_cdata;

	return(itor);
}

void
pr_itor_destroy(itor)
	pr_itor *itor;
{
	ASSERT(itor != NULL);

	FREE(itor);
}

#define RETVALID(itor)		return(itor->node != NULL)

int
pr_itor_valid(itor)
	const pr_itor *itor;
{
	ASSERT(itor != NULL);

	RETVALID(itor);
}

void
pr_itor_invalidate(itor)
	pr_itor *itor;
{
	ASSERT(itor != NULL);

	itor->node = NULL;
}

int
pr_itor_next(itor)
	pr_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->node == NULL)
		pr_itor_first(itor);
	else
		itor->node = node_next(itor->node);
	RETVALID(itor);
}

int
pr_itor_prev(itor)
	pr_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->node == NULL)
		pr_itor_last(itor);
	else
		itor->node = node_prev(itor->node);
	RETVALID(itor);
}

int
pr_itor_nextn(itor, count)
	pr_itor *itor;
	unsigned count;
{
	ASSERT(itor != NULL);

	if (count) {
		if (itor->node == NULL) {
			pr_itor_first(itor);
			count--;
		}

		while (count-- && itor->node)
			itor->node = node_next(itor->node);
	}

	RETVALID(itor);
}

int
pr_itor_prevn(itor, count)
	pr_itor *itor;
	unsigned count;
{
	ASSERT(itor != NULL);

	if (count) {
		if (itor->node == NULL) {
			pr_itor_last(itor);
			count--;
		}

		while (count-- && itor->node)
			itor->node = node_prev(itor->node);
	}

	RETVALID(itor);
}

int
pr_itor_first(itor)
	pr_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->tree->root == NULL)
		itor->node = NULL;
	else
		itor->node = node_min(itor->tree->root);
	RETVALID(itor);
}

int
pr_itor_last(itor)
	pr_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->tree->root == NULL)
		itor->node = NULL;
	else
		itor->node = node_max(itor->tree->root);
	RETVALID(itor);
}

const void *
pr_itor_key(itor)
	const pr_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->node ? itor->node->key : NULL);
}

void *
pr_itor_data(itor)
	pr_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->node ? itor->node->dat : NULL);
}

const void *
pr_itor_cdata(itor)
	const pr_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->node ? itor->node->dat : NULL);
}
