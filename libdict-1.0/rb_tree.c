/*
 * rb_tree.c
 *
 * Implementation of red-black binary search tree.
 * Copyright (C) 2001 Farooq Mela.
 *
 * $Id: rb_tree.c,v 1.2 2001/04/03 08:17:39 farooq Exp farooq $
 */

#include <stdlib.h>
#include <errno.h>

#include "rb_tree.h"
#include "dict_private.h"

typedef struct rb_node rb_node;
struct rb_node {
	void	*key;
	void	*dat;
	rb_node	*left;
	rb_node	*right;
	rb_node	*parent;
	unsigned color:1;
};

#define RB_RED			0
#define RB_BLK			1

struct rb_tree {
	rb_node	*root;
	unsigned count;
	dict_cmp_func key_cmp;
	dict_del_func key_del;
	dict_del_func dat_del;
};

struct rb_itor {
	rb_tree *tree;
	rb_node *node;
};

static rb_node nil = { NULL, NULL, NULL, NULL, NULL, RB_BLK };
#define RB_NIL	&nil

static void rot_left __P((rb_tree *tree, rb_node *node));
static void rot_right __P((rb_tree *tree, rb_node *node));
static void insert_fixup __P((rb_tree *tree, rb_node *node));
static void delete_fixup __P((rb_tree *tree, rb_node *node));
static unsigned node_height __P((const rb_node *node));
static unsigned node_mheight __P((const rb_node *node));
static rb_node *node_new __P((void *key, void *dat));
static rb_node *node_next __P((rb_node *node));
static rb_node *node_prev __P((rb_node *node));
static rb_node *node_max __P((rb_node *node));
static rb_node *node_min __P((rb_node *node));

rb_tree *
rb_tree_new(key_cmp, key_del, dat_del)
	dict_cmp_func	key_cmp;
	dict_del_func	key_del;
	dict_del_func	dat_del;
{
	rb_tree *tree;

	ASSERT(key_cmp != NULL);

	tree = MALLOC(sizeof(*tree));
	if (tree == NULL) {
		errno = ENOMEM;
		return(NULL);
	}

	tree->root = RB_NIL;
	tree->count = 0;
	tree->key_cmp = key_cmp;
	tree->key_del = key_del;
	tree->dat_del = dat_del;

	return(tree);
}

dict *
rb_dict_new(key_cmp, key_del, dat_del)
	dict_cmp_func	key_cmp;
	dict_del_func	key_del;
	dict_del_func	dat_del;
{
	dict *dct;
	rb_tree *tree;

	ASSERT(key_cmp != NULL);

	dct = MALLOC(sizeof(*dct));
	if (dct == NULL)
		return(NULL);

	tree = rb_tree_new(key_cmp, key_del, dat_del);
	if (tree == NULL) {
		FREE(dct);
		errno = ENOMEM;
		return(NULL);
	}

	dct->object = tree;
	dct->inew = (inew_func)rb_dict_itor_new;
	dct->destroy = (destroy_func)rb_tree_destroy;
	dct->insert = (insert_func)rb_tree_insert;
	dct->probe = (probe_func)rb_tree_probe;
	dct->search = (search_func)rb_tree_search;
	dct->csearch = (csearch_func)rb_tree_csearch;
	dct->remove = (remove_func)rb_tree_remove;
	dct->empty = (empty_func)rb_tree_empty;
	dct->walk = (walk_func)rb_tree_walk;
	dct->count = (count_func)rb_tree_count;

	return(dct);
}

void
rb_tree_destroy(tree, del)
	rb_tree	*tree;
	int del;
{
	ASSERT(tree != NULL);

	if (tree->root != RB_NIL)
		rb_tree_empty(tree, del);

	FREE(tree);
}

void *
rb_tree_search(tree, key)
	rb_tree		*tree;
	const void	*key;
{
	int rv;
	rb_node *node;

	ASSERT(tree != NULL);

	node = tree->root;
	while (node != RB_NIL) {
		rv = (*tree->key_cmp)(key, node->key);
		if (rv == 0)
			break;
		node = rv < 0 ? node->left : node->right;
	}

	return(node ? node->dat : NULL);
}

const void *
rb_tree_csearch(tree, key)
	const rb_tree	*tree;
	const void		*key;
{
	ASSERT(tree != NULL);

	return(rb_tree_search((rb_tree *)tree, key));
}

int
rb_tree_insert(tree, key, dat, overwrite)
	rb_tree	*tree;
	void	*key;
	void	*dat;
	int		 overwrite;
{
	int rv = 0;
	rb_node *node, *parent = RB_NIL;

	ASSERT(tree != NULL);

	node = tree->root;

	while (node != RB_NIL) {
		rv = (*tree->key_cmp)(key, node->key);
		if (rv == 0) {
			if (overwrite == 0)
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
	if (parent == RB_NIL) {
		tree->root = node;
	} else {
		if (rv < 0)
			parent->left = node;
		else
			parent->right = node;
	}
	insert_fixup(tree, node);

	tree->count++;
	return(0);
}

int
rb_tree_probe(tree, key, dat)
	rb_tree		 *tree;
	void		 *key;
	void		**dat;
{
	int rv = 0;
	rb_node *node, *parent = RB_NIL;

	ASSERT(tree != NULL);

	node = tree->root;

	while (node != RB_NIL) {
		rv = (*tree->key_cmp)(key, node->key);
		if (rv == 0) {
			*dat = node->dat;
			return(0);
		}

		parent = node;
		node = rv < 0 ? node->left : node->right;
	}

	node = node_new(key, *dat);
	if (node == NULL)
		return(-1);

	node->parent = parent;
	if (parent == RB_NIL) {
		tree->root = node;
	} else {
		if (rv < 0)
			parent->left = node;
		else
			parent->right = node;
	}

	insert_fixup(tree, node);
	tree->count++;
	return(1);
}

void
insert_fixup(tree, node)
	rb_tree	*tree;
	rb_node	*node;
{
	rb_node *temp;

	ASSERT(tree != NULL);
	ASSERT(node != NULL);

	while (node != tree->root && node->parent->color == RB_RED) {
		if (node->parent == node->parent->parent->left) {
			temp = node->parent->parent->right;
			if (temp->color == RB_RED) {
				temp->color = RB_BLK;
				node->parent->color = RB_BLK;
				node->parent->parent->color = RB_RED;
				node = node->parent->parent;
			} else {
				if (node == node->parent->right) {
					node = node->parent;
					rot_left(tree, node);
				}
				node->parent->color = RB_BLK;
				node->parent->parent->color = RB_RED;
				rot_right(tree, node->parent->parent);
			}
		} else {
			temp = node->parent->parent->left;
			if (temp->color == RB_RED) {
				temp->color = RB_BLK;
				node->parent->color = RB_BLK;
				node->parent->parent->color = RB_RED;
				node = node->parent->parent;
			} else {
				if (node == node->parent->left) {
					node = node->parent;
					rot_right(tree, node);
				}
				node->parent->color = RB_BLK;
				node->parent->parent->color = RB_RED;
				rot_left(tree, node->parent->parent);
			}
		}
	}

	tree->root->color = RB_BLK;
}

int
rb_tree_remove(tree, key, del)
	rb_tree		*tree;
	const void	*key;
	int			 del;
{
	int rv;
	rb_node *node, *temp, *out;

	ASSERT(tree != NULL);

	node = tree->root;
	while (node != RB_NIL) {
		rv = (*tree->key_cmp)(key, node->key);
		if (rv == 0)
			break;
		node = rv < 0 ? node->left : node->right;
	}
	if (node == NULL)
		return(-1);

	if (node->left == RB_NIL || node->right == RB_NIL) {
		out = node;
	} else {
		for (out = node->right; out->left != RB_NIL; out = out->left)
			/* void */;
		SWAP(node->key, out->key, void *);
		SWAP(node->dat, out->dat, void *);
	}

	temp = out->left != RB_NIL ? out->left : out->right;
	temp->parent = out->parent;
	if (out->parent != RB_NIL) {
		if (out->parent->left == out)
			out->parent->left = temp;
		else
			out->parent->right = temp;
	} else {
		tree->root = temp;
	}

	if (out->color == RB_BLK)
		delete_fixup(tree, temp);
	if (del) {
		if (tree->key_del)
			(*tree->key_del)(out->key);
		if (tree->dat_del)
			(*tree->dat_del)(out->dat);
	}
	FREE(out);

	tree->count--;

	return(0);
}

static void
delete_fixup(tree, node)
	rb_tree *tree;
	rb_node *node;
{
	rb_node *temp;

	ASSERT(tree != NULL);
	ASSERT(node != NULL);

	while (node != tree->root && node->color == RB_BLK) {
		if (node->parent->left == node) {
			temp = node->parent->right;
			if (temp->color == RB_RED) {
				temp->color = RB_BLK;
				node->parent->color = RB_RED;
				rot_left(tree, node->parent);
				temp = node->parent->right;
			}
			if (temp->left->color == RB_BLK && temp->right->color == RB_BLK) {
				temp->color = RB_RED;
				node = node->parent;
			} else {
				if (temp->right->color == RB_BLK) {
					temp->left->color = RB_BLK;
					temp->color = RB_RED;
					rot_right(tree, temp);
					temp = node->parent->right;
				}
				temp->color = node->parent->color;
				temp->right->color = RB_BLK;
				node->parent->color = RB_BLK;
				rot_left(tree, node->parent);
				node = tree->root;
			}
		} else {
			temp = node->parent->left;
			if (temp->color == RB_RED) {
				temp->color = RB_BLK;
				node->parent->color = RB_RED;
				rot_right(tree, node->parent);
				temp = node->parent->left;
			}
			if (temp->right->color == RB_BLK && temp->left->color == RB_BLK) {
				temp->color = RB_RED;
				node = node->parent;
			} else {
				if (temp->left->color == RB_BLK) {
					temp->right->color = RB_BLK;
					temp->color = RB_RED;
					rot_left(tree, temp);
					temp = node->parent->left;
				}
				temp->color = node->parent->color;
				node->parent->color = RB_BLK;
				temp->left->color = RB_BLK;
				rot_right(tree, node->parent);
				node = tree->root;
			}
		}
	}

	node->color = RB_BLK;
}

void
rb_tree_empty(tree, del)
	rb_tree		*tree;
	int			 del;
{
	rb_node *node, *parent;

	ASSERT(tree != NULL);

	node = tree->root;
	while (node != RB_NIL) {
		parent = node->parent;
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

unsigned
rb_tree_count(tree)
	const rb_tree *tree;
{
	ASSERT(tree != NULL);

	return(tree->count);
}

unsigned
rb_tree_height(tree)
	const rb_tree *tree;
{
	ASSERT(tree != NULL);

	return(tree->root != RB_NIL ? node_height(tree->root) : 0);
}

unsigned
rb_tree_mheight(tree)
	const rb_tree *tree;
{
	ASSERT(tree != NULL);

	return(tree->root != RB_NIL ? node_mheight(tree->root) : 0);
}

const void *
rb_tree_min(tree)
	const rb_tree *tree;
{
	const rb_node *node;

	ASSERT(tree != NULL);

	if (tree->root == RB_NIL)
		return(NULL);

	for (node = tree->root; node->left; node = node->left)
		/* void */;
	return(node->key);
}

const void *
rb_tree_max(tree)
	const rb_tree *tree;
{
	const rb_node *node;

	ASSERT(tree != NULL);

	if (tree->root == RB_NIL)
		return(NULL);

	for (node = tree->root; node->right; node = node->right)
		/* void */;
	return(node->key);
}

void
rb_tree_walk(tree, visit)
	rb_tree *tree;
	dict_vis_func visit;
{
	rb_node *node;

	ASSERT(tree != NULL);
	ASSERT(visit != NULL);

	if (tree->root == RB_NIL)
		return;

	for (node = node_min(tree->root); node != RB_NIL; node = node_next(node))
		if ((*visit)(node->key, node->dat) == 0)
			break;
}

static unsigned
node_height(node)
	const rb_node *node;
{
	unsigned l, r;

	l = node->left != RB_NIL ? node_height(node->left) + 1 : 0;
	r = node->right != RB_NIL ? node_height(node->right) + 1 : 0;
	return(MAX(l, r));
}

static unsigned
node_mheight(node)
	const rb_node *node;
{
	unsigned l, r;

	l = node->left != RB_NIL ? node_mheight(node->left) + 1 : 0;
	r = node->right != RB_NIL ? node_mheight(node->right) + 1 : 0;
	return(MIN(l, r));
}

static void
rot_left(tree, node)
	rb_tree	*tree;
	rb_node	*node;
{
	rb_node *right;

	ASSERT(tree != NULL);
	ASSERT(node != NULL);

	right = node->right;
	node->right = right->left;
	if (right->left != RB_NIL)
		right->left->parent = node;
	right->parent = node->parent;
	if (node->parent == RB_NIL)
		tree->root = right;
	else {
		if (node->parent->left == node)
			node->parent->left = right;
		else
			node->parent->right = right;
	}
	right->left = node;
	node->parent = right;
}

static void
rot_right(tree, node)
	rb_tree	*tree;
	rb_node	*node;
{
	rb_node *left;

	ASSERT(tree != NULL);
	ASSERT(node != NULL);

	left = node->left;
	node->left = left->right;
	if (left->right != RB_NIL)
		left->right->parent = node;
	left->parent = node->parent;
	if (node->parent == RB_NIL)
		tree->root = left;
	else {
		if (node->parent->left == node)
			node->parent->left = left;
		else
			node->parent->right = left;
	}
	left->right = node;
	node->parent = left;
}

static rb_node *
node_new(key, dat)
	void *key;
	void *dat;
{
	rb_node *node;

	node = MALLOC(sizeof(*node));
	if (node == NULL)
		return(NULL);

	node->key = key;
	node->dat = dat;
	node->color = RB_RED;
	node->left = RB_NIL;
	node->right = RB_NIL;

	return(node);
}

static rb_node *
node_next(node)
	rb_node *node;
{
	rb_node *temp;

	ASSERT(node != NULL);

	if (node->right != RB_NIL) {
		for (node = node->right; node->left != RB_NIL; node = node->left)
			/* void */;
	} else {
		temp = node->parent;
		while (temp != RB_NIL && temp->right == node) {
			node = temp;
			temp = temp->parent;
		}
		node = temp;
	}

	return(node);
}

static rb_node *
node_prev(node)
	rb_node *node;
{
	rb_node *temp;

	ASSERT(node != NULL);

	if (node->left != RB_NIL) {
		for (node = node->left; node->right != RB_NIL; node = node->right)
			/* void */;
	} else {
		temp = node->parent;
		while (temp != RB_NIL && temp->left == node) {
			node = temp;
			temp = temp->parent;
		}
		node = temp;
	}

	return(node);
}

static rb_node *
node_max(node)
	rb_node *node;
{
	ASSERT(node != NULL);

	while (node->right != RB_NIL)
		node = node->right;
	return(node);
}

static rb_node *
node_min(node)
	rb_node *node;
{
	ASSERT(node != NULL);

	while (node->left != RB_NIL)
		node = node->left;
	return(node);
}

rb_itor *
rb_itor_new(tree)
	rb_tree *tree;
{
	rb_itor *itor;

	ASSERT(tree != NULL);

	itor = MALLOC(sizeof(*itor));
	if (itor == NULL)
		return(NULL);

	itor->tree = tree;
	rb_itor_first(itor);
	return(itor);
}

dict_itor *
rb_dict_itor_new(tree)
	rb_tree *tree;
{
	dict_itor *itor;

	ASSERT(tree != NULL);

	itor = MALLOC(sizeof(*itor));
	if (itor == NULL)
		return(NULL);

	itor->itor = rb_itor_new(tree);
	if (!itor->itor) {
		FREE(itor);
		errno = ENOMEM;
		return(NULL);
	}

	itor->destroy = (idestroy_func)rb_itor_destroy;
	itor->valid = (valid_func)rb_itor_valid;
	itor->invalid = (invalidate_func)rb_itor_invalidate;
	itor->next = (next_func)rb_itor_next;
	itor->prev = (prev_func)rb_itor_prev;
	itor->nextn = (nextn_func)rb_itor_nextn;
	itor->prevn = (prevn_func)rb_itor_prevn;
	itor->first = (first_func)rb_itor_first;
	itor->last = (last_func)rb_itor_last;
	itor->key = (key_func)rb_itor_key;
	itor->data = (data_func)rb_itor_data;
	itor->cdata = (cdata_func)rb_itor_cdata;

	return(itor);
}

void
rb_itor_destroy(itor)
	rb_itor *itor;
{
	ASSERT(itor != NULL);

	FREE(itor);
}

#define RETVALID(itor)		return(itor->node != RB_NIL)

int
rb_itor_valid(itor)
	const rb_itor *itor;
{
	ASSERT(itor != NULL);

	RETVALID(itor);
}

void
rb_itor_invalidate(itor)
	rb_itor *itor;
{
	ASSERT(itor != NULL);

	itor->node = RB_NIL;
}

int
rb_itor_next(itor)
	rb_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->node == RB_NIL)
		rb_itor_first(itor);
	else
		itor->node = node_next(itor->node);
	RETVALID(itor);
}

int
rb_itor_prev(itor)
	rb_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->node == RB_NIL)
		rb_itor_last(itor);
	else
		itor->node = node_prev(itor->node);
	RETVALID(itor);
}

int
rb_itor_nextn(itor, count)
	rb_itor *itor;
	unsigned count;
{
	ASSERT(itor != NULL);

	if (count) {
		if (itor->node == RB_NIL) {
			rb_itor_first(itor);
			count--;
		}

		while (count-- && itor->node)
			itor->node = node_next(itor->node);
	}

	RETVALID(itor);
}

int
rb_itor_prevn(itor, count)
	rb_itor *itor;
	unsigned count;
{
	ASSERT(itor != NULL);

	if (count) {
		if (itor->node == RB_NIL) {
			rb_itor_last(itor);
			count--;
		}

		while (count-- && itor->node)
			itor->node = node_prev(itor->node);
	}

	RETVALID(itor);
}

int
rb_itor_first(itor)
	rb_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->tree->root == RB_NIL)
		itor->node = RB_NIL;
	else
		itor->node = node_min(itor->tree->root);
	RETVALID(itor);
}

int
rb_itor_last(itor)
	rb_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->tree->root == RB_NIL)
		itor->node = RB_NIL;
	else
		itor->node = node_max(itor->tree->root);
	RETVALID(itor);
}

const void *
rb_itor_key(itor)
	const rb_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->node != RB_NIL ? itor->node->key : NULL);
}

void *
rb_itor_data(itor)
	rb_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->node != RB_NIL ? itor->node->dat : NULL);
}

const void *
rb_itor_cdata(itor)
	const rb_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->node != RB_NIL ? itor->node->dat : NULL);
}

