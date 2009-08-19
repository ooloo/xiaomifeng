/*
 * wb_tree.c
 *
 * Implementation of weight balanced tree.
 * Copyright (C) 2001 Farooq Mela.
 *
 * $Id: wb_tree.c,v 1.1 2001/04/23 01:12:32 farooq Exp farooq $
 */

#include <stdlib.h>
#include <setjmp.h>
#include <errno.h>

#include "wb_tree.h"
#include "dict_private.h"

typedef unsigned long wt_t;

/*
 * Cf. G.H. Gonnet, Handbook of Algorithms and Data Structures
 *
 * A tree BB[alpha] is said to be of weighted balance alpha if every node in
 * the tree has a balance p(n) such that alpha <= p(n) <= 1 - alpha. The
 * balance of a node is defined as the number of nodes in its left subtree
 * divided by the number of nodes in either subtree. The weight of a node is
 * defined as the number of external nodes in its subtrees.
 *
 * Legal values for alpha are 0 <= alpha <= 1/2. BB[0] is a normal, unbalanced
 * binary tree, and BB[1/2] includes only completely balanced binary search
 * trees of 2^height - 1 nodes. A higher value of alpha specifies a more
 * stringent balance requirement. Values for alpha in the range 2/11 <= alpha
 * <= 1 - sqrt(2)/2 are interesting because a tree can be brought back into
 * weighted balance after an insertion or deletion using at most rotation per
 * level.
 *
 * These are the parameters for alpha = 1 - sqrt(2)/2 == .292893
 * These are truncated, not rounded, values. The second value is equal to
 * sqrt(2)/2, and the last value is equal to 2 - sqrt(2), but they are left
 * unreduced to make it more obvious how those values were derived.
 *
 * The author of this library has experimented with these values to some extent
 * and found that this value of alpha typically gives the best performance.
 *
 * There is no significance in the naming of the macros.
 */
#define ALPHA_0		.292893		/* 1 - sqrt(2)/2		*/
#define ALPHA_1		.707106		/* 1 - (1 - sqrt(2)/2)	*/
#define ALPHA_2		.414213		/* sqrt(2) - 1			*/
#define ALPHA_3		.585786		/* 1 - (sqrt(2) - 1)	*/

typedef struct wb_node wb_node;
struct wb_node {
	void	*key;
	void	*dat;
	wb_node	*parent;
	wb_node	*left;
	wb_node	*right;
	wt_t	 weight;
};

#define WEIGHT(n)	((n) ? (n)->weight : 1)

struct wb_tree {
	wb_node			*root;
	unsigned		 count;
	dict_cmp_func	 key_cmp;
	dict_del_func	 key_del;
	dict_del_func	 dat_del;
};

struct wb_itor {
	wb_tree	*tree;
	wb_node	*node;
};

static int delete __P((wb_tree *tree, wb_node **node, const void *key,
					   int del));
static void rot_left __P((wb_tree *tree, wb_node *node));
static void rot_right __P((wb_tree *tree, wb_node *node));
static unsigned node_height __P((const wb_node *node));
static unsigned node_mheight __P((const wb_node *node));
static wb_node *node_new __P((void *key, void *dat));
static wb_node *node_min __P((wb_node *node));
static wb_node *node_max __P((wb_node *node));
static wb_node *node_next __P((wb_node *node));
static wb_node *node_prev __P((wb_node *node));

wb_tree *
wb_tree_new(key_cmp, key_del, dat_del)
	dict_cmp_func	key_cmp;
	dict_del_func	key_del;
	dict_del_func	dat_del;
{
	wb_tree *tree;

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
wb_dict_new(key_cmp, key_del, dat_del)
	dict_cmp_func	key_cmp;
	dict_del_func	key_del;
	dict_del_func	dat_del;
{
	dict *dct;
	wb_tree *tree;

	ASSERT(key_cmp != NULL);

	dct = MALLOC(sizeof(*dct));
	if (dct == NULL) {
		errno = ENOMEM;
		return(NULL);
	}

	tree = wb_tree_new(key_cmp, key_del, dat_del);
	if (tree == NULL) {
		FREE(dct);
		errno = ENOMEM;
		return(NULL);
	}

	dct->object = tree;
	dct->inew = (inew_func)wb_dict_itor_new;
	dct->destroy = (destroy_func)wb_tree_destroy;
	dct->insert = (insert_func)wb_tree_insert;
	dct->probe = (probe_func)wb_tree_probe;
	dct->search = (search_func)wb_tree_search;
	dct->csearch = (csearch_func)wb_tree_csearch;
	dct->remove = (remove_func)wb_tree_remove;
	dct->empty = (empty_func)wb_tree_empty;
	dct->walk = (walk_func)wb_tree_walk;
	dct->count = (count_func)wb_tree_count;

	return(dct);
}

void
wb_tree_destroy(tree, del)
	wb_tree	*tree;
	int del;
{
	ASSERT(tree != NULL);

	if (tree->root)
		wb_tree_empty(tree, del);

	FREE(tree);
}

void *
wb_tree_search(tree, key)
	wb_tree		*tree;
	const void	*key;
{
	int rv;
	wb_node *node;

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
wb_tree_csearch(tree, key)
	const wb_tree	*tree;
	const void		*key;
{
	int rv;
	const wb_node *node;

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
wb_tree_insert(tree, key, dat, overwrite)
	wb_tree		*tree;
	void		*key;
	void		*dat;
	int			 overwrite;
{
	int rv = 0;
	wb_node *node, *parent = NULL;
	float wbal;

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
		parent = node->parent;
		node->weight = WEIGHT(node->left) + WEIGHT(node->right);
		wbal = WEIGHT(node->left) / (float)node->weight;
		if (wbal < ALPHA_0) {
			if (wbal < ALPHA_3) {		/* LL */
				rot_left(tree, node);
			} else {					/* RL */
				rot_right(tree, node->right);
				rot_left(tree, node);
			}
		} else if (wbal > ALPHA_1) {
			wbal = WEIGHT(node->left->left) / (float)node->left->weight;
			if (wbal > ALPHA_2) {		/* RR */
				rot_right(tree, node);
			} else {					/* LR */
				rot_left(tree, node->left);
				rot_right(tree, node);
			}
		}
	}
	tree->count++;
	return(0);
}

int
wb_tree_probe(tree, key, dat)
	wb_tree		 *tree;
	void		 *key;
	void		**dat;
{
	int rv = 0;
	wb_node *node, *parent = NULL;
	float wbal;

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
		parent = node->parent;
		node->weight = WEIGHT(node->left) + WEIGHT(node->right);
		wbal = WEIGHT(node->left) / (float)node->weight;
		if (wbal < ALPHA_0) {
			if (wbal < ALPHA_3) {		/* LL */
				rot_left(tree, node);
			} else {					/* RL */
				rot_right(tree, node->right);
				rot_left(tree, node);
			}
		} else if (wbal > ALPHA_1) {
			wbal = WEIGHT(node->left->left) / (float)node->left->weight;
			if (wbal > ALPHA_2) {		/* RR */
				rot_right(tree, node);
			} else {					/* LR */
				rot_left(tree, node->left);
				rot_right(tree, node);
			}
		}
	}
	tree->count++;
	return(1);
}

int
wb_tree_remove(tree, key, del)
	wb_tree		*tree;
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
	wb_tree		 *tree;
	wb_node		**node;
	const void	 *key;
	int			  del;
{
	int rv;
	wb_node *out;

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
wb_tree_empty(tree, del)
	wb_tree *tree;
	int		 del;
{
	wb_node *node, *parent;

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
wb_tree_min(tree)
	const wb_tree *tree;
{
	const wb_node *node;

	ASSERT(tree != NULL);

	if (tree->root == NULL)
		return(NULL);

	for (node = tree->root; node->left; node = node->left)
		/* void */;
	return(node->key);
}

const void *
wb_tree_max(tree)
	const wb_tree *tree;
{
	const wb_node *node;

	ASSERT(tree != NULL);

	if (tree->root == NULL)
		return(NULL);

	for (node = tree->root; node->right; node = node->left)
		/* void */;
	return(node->key);
}

void
wb_tree_walk(tree, visit)
	wb_tree			*tree;
	dict_vis_func	 visit;
{
	wb_node *node;

	ASSERT(tree != NULL);

	if (tree->root == NULL)
		return;
	for (node = node_min(tree->root); node; node = node_next(node))
		if ((*visit)(node->key, node->dat) == 0)
			break;
}

unsigned
wb_tree_count(tree)
	const wb_tree *tree;
{
	ASSERT(tree != NULL);

	return(tree->count);
}

unsigned
wb_tree_height(tree)
	const wb_tree *tree;
{
	ASSERT(tree != NULL);

	return(tree->root ? node_height(tree->root) : 0);
}

unsigned
wb_tree_mheight(tree)
	const wb_tree *tree;
{
	ASSERT(tree != NULL);

	return(tree->root ? node_mheight(tree->root) : 0);
}

static wb_node *
node_new(key, dat)
	void *key;
	void *dat;
{
	wb_node *node;

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

static wb_node *
node_min(node)
	wb_node *node;
{
	ASSERT(node != NULL);

	while (node->left)
		node = node->left;
	return(node);
}

static wb_node *
node_max(node)
	wb_node *node;
{
	ASSERT(node != NULL);

	while (node->right)
		node = node->right;
	return(node);
}

static wb_node *
node_next(node)
	wb_node		*node;
{
	wb_node *temp;

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

static wb_node *
node_prev(node)
	wb_node		*node;
{
	wb_node *temp;

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
	const wb_node *node;
{
	unsigned l, r;

	ASSERT(node != NULL);

	l = node->left ? node_height(node->left) + 1 : 0;
	r = node->right ? node_height(node->right) + 1 : 0;
	return(MAX(l, r));
}

static unsigned
node_mheight(node)
	const wb_node *node;
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
	wb_tree	*tree;
	wb_node	*node;
{
	wb_node *right;

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
	wb_tree	*tree;
	wb_node	*node;
{
	wb_node *left;

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

wb_itor *
wb_itor_new(tree)
	wb_tree *tree;
{
	wb_itor *itor;

	ASSERT(tree != NULL);

	itor = MALLOC(sizeof(*itor));
	if (itor == NULL) {
		errno = ENOMEM;
		return(NULL);
	}

	itor->tree = tree;
	wb_itor_first(itor);
	return(itor);
}

dict_itor *
wb_dict_itor_new(tree)
	wb_tree *tree;
{
	dict_itor *itor;

	ASSERT(tree != NULL);

	itor = MALLOC(sizeof(*itor));
	if (itor == NULL) {
		errno = ENOMEM;
		return(NULL);
	}

	itor->itor = wb_itor_new(tree);
	if (!itor->itor) {
		FREE(itor);
		return(NULL);
	}

	itor->destroy = (idestroy_func)wb_itor_destroy;
	itor->valid = (valid_func)wb_itor_valid;
	itor->invalid = (invalidate_func)wb_itor_invalidate;
	itor->next = (next_func)wb_itor_next;
	itor->prev = (prev_func)wb_itor_prev;
	itor->nextn = (nextn_func)wb_itor_nextn;
	itor->prevn = (prevn_func)wb_itor_prevn;
	itor->first = (first_func)wb_itor_first;
	itor->last = (last_func)wb_itor_last;
	itor->key = (key_func)wb_itor_key;
	itor->data = (data_func)wb_itor_data;
	itor->cdata = (cdata_func)wb_itor_cdata;

	return(itor);
}

void
wb_itor_destroy(itor)
	wb_itor *itor;
{
	ASSERT(itor != NULL);

	FREE(itor);
}

#define RETVALID(itor)		return(itor->node != NULL)

int
wb_itor_valid(itor)
	const wb_itor *itor;
{
	ASSERT(itor != NULL);

	RETVALID(itor);
}

void
wb_itor_invalidate(itor)
	wb_itor *itor;
{
	ASSERT(itor != NULL);

	itor->node = NULL;
}

int
wb_itor_next(itor)
	wb_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->node == NULL)
		wb_itor_first(itor);
	else
		itor->node = node_next(itor->node);
	RETVALID(itor);
}

int
wb_itor_prev(itor)
	wb_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->node == NULL)
		wb_itor_last(itor);
	else
		itor->node = node_prev(itor->node);
	RETVALID(itor);
}

int
wb_itor_nextn(itor, count)
	wb_itor *itor;
	unsigned count;
{
	ASSERT(itor != NULL);

	if (count) {
		if (itor->node == NULL) {
			wb_itor_first(itor);
			count--;
		}

		while (count-- && itor->node)
			itor->node = node_next(itor->node);
	}

	RETVALID(itor);
}

int
wb_itor_prevn(itor, count)
	wb_itor *itor;
	unsigned count;
{
	ASSERT(itor != NULL);

	if (count) {
		if (itor->node == NULL) {
			wb_itor_last(itor);
			count--;
		}

		while (count-- && itor->node)
			itor->node = node_prev(itor->node);
	}

	RETVALID(itor);
}

int
wb_itor_first(itor)
	wb_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->tree->root == NULL)
		itor->node = NULL;
	else
		itor->node = node_min(itor->tree->root);
	RETVALID(itor);
}

int
wb_itor_last(itor)
	wb_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->tree->root == NULL)
		itor->node = NULL;
	else
		itor->node = node_max(itor->tree->root);
	RETVALID(itor);
}

const void *
wb_itor_key(itor)
	const wb_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->node ? itor->node->key : NULL);
}

void *
wb_itor_data(itor)
	wb_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->node ? itor->node->dat : NULL);
}

const void *
wb_itor_cdata(itor)
	const wb_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->node ? itor->node->dat : NULL);
}
