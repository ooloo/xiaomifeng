/*
 * sp_tree.c
 *
 * Implementation for splay binary search tree.
 * Copyright (C) 2001 Farooq Mela.
 *
 * A single operation on a splay tree has a worst-case time complexity of O(N),
 * but a series of N operations have a time complexity of O(N lg N), and thus
 * the amortized time complexity of an operation is O(lg N). More specifically,
 * a series of M operations on a tree with N nodes will take time proportional
 * to (N+M)lg(N+M). Splay trees work by "splaying" a node up the tree until it
 * is the root each time it is accessed. They are much simpler to code than
 * most balanced trees, because there is no strict requirement about
 * maintaining a balance among nodes. When inserting and searching, we simply
 * splay the node in question up until it becomes the root of the tree.
 *
 * This implementation is a bottom-up, move-to-root splay tree.
 *
 * $Id: sp_tree.c,v 1.1 2001/04/11 05:11:14 farooq Exp farooq $
 */

#include <stdlib.h>
#include <errno.h>

#include "sp_tree.h"
#include "dict_private.h"

typedef struct sp_node sp_node;
struct sp_node {
	void	*key;
	void	*dat;
	sp_node	*left;
	sp_node	*right;
	sp_node	*parent;
};

struct sp_tree {
	sp_node	*root;
	unsigned count;
	dict_cmp_func key_cmp;
	dict_del_func key_del;
	dict_del_func dat_del;
};

struct sp_itor {
	sp_tree	*tree;
	sp_node	*node;
};

static void rot_left __P((sp_tree *tree, sp_node *node));
static void rot_right __P((sp_tree *tree, sp_node *node));
static void zig_zig_right __P((sp_tree *tree, sp_node *node));
static void zig_zig_left __P((sp_tree *tree, sp_node *node));
static void zig_zag_right __P((sp_tree *tree, sp_node *node));
static void zig_zag_left __P((sp_tree *tree, sp_node *node));
static void splay __P((sp_tree *tree, sp_node *node));

static sp_node *node_new __P((void *key, void *dat));
static sp_node *node_next __P((sp_node *node));
static sp_node *node_prev __P((sp_node *node));
static sp_node *node_max __P((sp_node *node));
static sp_node *node_min __P((sp_node *node));
static unsigned node_height __P((const sp_node *node));
static unsigned node_mheight __P((const sp_node *node));

sp_tree *
sp_tree_new(key_cmp, key_del, dat_del)
	dict_cmp_func	key_cmp;
	dict_del_func	key_del;
	dict_del_func	dat_del;
{
	sp_tree *tree;

	ASSERT(key_cmp != NULL);

	tree = MALLOC(sizeof(*tree));
	if (tree == NULL)
		return(NULL);

	tree->root = NULL;
	tree->count = 0;
	tree->key_cmp = key_cmp;
	tree->key_del = key_del;
	tree->dat_del = dat_del;
	return(tree);
}

dict *
sp_dict_new(key_cmp, key_del, dat_del)
	dict_cmp_func	key_cmp;
	dict_del_func	key_del;
	dict_del_func	dat_del;
{
	dict *dct;

	ASSERT(key_cmp != NULL);

	dct = MALLOC(sizeof(*dct));
	if (dct == NULL)
		return(NULL);

	dct->object = sp_tree_new(key_cmp, key_del, dat_del);
	if (dct->object == NULL) {
		FREE(dct);
		errno = ENOMEM;
		return(NULL);
	}

	dct->inew = (inew_func)sp_dict_itor_new;
	dct->destroy = (destroy_func)sp_tree_destroy;
	dct->insert = (insert_func)sp_tree_insert;
	dct->probe = (probe_func)sp_tree_probe;
	dct->search = (search_func)sp_tree_search;
	dct->csearch = (csearch_func)sp_tree_csearch;
	dct->remove = (remove_func)sp_tree_remove;
	dct->empty = (empty_func)sp_tree_empty;
	dct->walk = (walk_func)sp_tree_walk;
	dct->count = (count_func)sp_tree_count;

	return(dct);
}

void
sp_tree_destroy(tree, del)
	sp_tree	*tree;
	int		 del;
{
	ASSERT(tree != NULL);

	if (tree->root)
		sp_tree_empty(tree, del);
	FREE(tree);
}

void
sp_tree_empty(tree, del)
	sp_tree		*tree;
	int			 del;
{
	sp_node *node, *parent;

	ASSERT(tree != NULL);

	node = tree->root;
	while (node) {
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

int
sp_tree_insert(tree, key, dat, overwrite)
	sp_tree	*tree;
	void	*key;
	void	*dat;
	int		 overwrite;
{
	int rv = 0; /* shut up GCC */
	sp_node *node, *parent = NULL;

	ASSERT(tree != NULL);

	node = tree->root;
	while (node) {
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
	if (parent == NULL) {
		ASSERT(tree->count == 0);
		tree->root = node;
	} else {
		if (rv < 0)
			parent->left = node;
		else
			parent->right = node;
	}
	tree->count++;

	while (tree->root != node)
		splay(tree, node);

	return(0);
}

int
sp_tree_probe(tree, key, dat)
	sp_tree	 *tree;
	void	 *key;
	void	**dat;
{
	int rv = 0;
	sp_node *node, *parent = NULL;

	ASSERT(tree != NULL);

	node = tree->root;
	while (node) {
		rv = (*tree->key_cmp)(key, node->key);
		if (rv == 0) {
			while (tree->root != node)
				splay(tree, node);
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
	if (parent == NULL) {
		ASSERT(tree->count == 0);
		tree->root = node;
	} else {
		if (rv < 0)
			parent->left = node;
		else
			parent->right = node;
	}
	tree->count++;

	while (tree->root != node)
		splay(tree, node);

	return(1);
}

void *
sp_tree_search(tree, key)
	sp_tree		*tree;
	const void	*key;
{
	int rv;
	sp_node *node;

	ASSERT(tree != NULL);

	node = tree->root;
	while (node) {
		rv = (*tree->key_cmp)(key, node->key);
		if (rv == 0) {
			while (tree->root != node)
				splay(tree, node);
			return(node->dat);
		}
		node = rv < 0 ? node->left : node->right;
	}
	return(NULL);
}

const void *
sp_tree_csearch(tree, key)
	const sp_tree	*tree;
	const void		*key;
{
	/*
	 * This cast is OK, because contents of tree remain same, it is only the
	 * relative "ordering" that changes with splaying.
	 */
	return(sp_tree_search((sp_tree *)tree, key));
}

int
sp_tree_remove(tree, key, del)
	sp_tree		*tree;
	const void	*key;
	int			 del;
{
	int rv;
	sp_node *node, *temp, *out;

	ASSERT(tree != NULL);

	node = tree->root;
	while (node) {
		rv = (*tree->key_cmp)(key, node->key);
		if (rv == 0)
			break;
		node = rv < 0 ? node->left : node->right;
	}

	if (node == NULL)
		return(-1);

	if (node->left == NULL || node->right == NULL) {
		out = node;
	} else {
		/*
		 * This is sure to screw up iterators that were positioned at the node
		 * "out".
		 */
		for (out = node->right; out->left; out = out->left)
			/* void */;
		SWAP(node->key, out->key, void *);
		SWAP(node->dat, out->dat, void *);
	}

	temp = out->left ? out->left : out->right;
	if (temp)
		temp->parent = out->parent;
	if (out->parent) {
		if (out->parent->left == out)
			out->parent->left = temp;
		else
			out->parent->right = temp;
	} else {
		tree->root = temp;
	}
	if (del) {
		if (tree->key_del)
			(*tree->key_del)(out->key);
		if (tree->dat_del)
			(*tree->dat_del)(out->dat);
	}

	if (node->parent)
		temp = node->parent;
	else if (node->left)
		temp = node->left;
	else
		temp = node->right;
	if (temp)
		while (tree->root != temp)
			splay(tree, temp);

	FREE(out);
	tree->count--;

	return(0);
}

void
sp_tree_walk(tree, visit)
	sp_tree			*tree;
	dict_vis_func	 visit;
{
	sp_node *node;

	ASSERT(tree != NULL);
	ASSERT(visit != NULL);

	if (tree->root == NULL)
		return;

	for (node = node_min(tree->root); node; node = node_next(node))
		if ((*visit)(node->key, node->dat) == 0)
			break;
}

unsigned
sp_tree_count(tree)
	const sp_tree	*tree;
{
	ASSERT(tree != NULL);

	return(tree->count);
}

unsigned
sp_tree_height(tree)
	const sp_tree	*tree;
{
	ASSERT(tree != NULL);

	return(tree->root ? node_height(tree->root) : 0);
}

unsigned
sp_tree_mheight(tree)
	const sp_tree	*tree;
{
	ASSERT(tree != NULL);

	return(tree->root ? node_mheight(tree->root) : 0);
}

const void *
sp_tree_min(tree)
	const sp_tree *tree;
{
	const sp_node *node;

	ASSERT(tree != NULL);

	if (tree->root == NULL)
		return(NULL);

	for (node = tree->root; node->left; node = node->left)
		/* void */;
	return(node->key);
}

const void *
sp_tree_max(tree)
	const sp_tree *tree;
{
	const sp_node *node;

	ASSERT(tree != NULL);

	if (tree->root == NULL)
		return(NULL);

	for (node = tree->root; node->right; node = node->right)
		/* void */;
	return(node->key);
}

static void
rot_left(tree, node)
	sp_tree	*tree;
	sp_node	*node;
{
	sp_node *right;

	ASSERT(tree != NULL);
	ASSERT(node != NULL);

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
}

static void
rot_right(tree, node)
	sp_tree	*tree;
	sp_node	*node;
{
	sp_node *left;

	ASSERT(tree != NULL);
	ASSERT(node != NULL);

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
}

/*
 * XXX Each zig/zig and zig/zag operation can be optimized, but for now we just
 * use two rotations.
 */

/*
 * zig_zig_right(T, A):
 *
 *     C               A
 *    /        B        \
 *   B   ==>  / \  ==>   B
 *  /        A   C        \
 * A                       C
 */
static void
zig_zig_right(tree, node)
	sp_tree *tree;
	sp_node *node;
{
	ASSERT(tree != NULL);
	ASSERT(node != NULL);
	ASSERT(node->parent != NULL);
	ASSERT(node->parent->parent != NULL);

	rot_right(tree, node->parent->parent);
	rot_right(tree, node->parent);
}

/*
 * zig_zig_left(T, C):
 *
 * A                        C
 *  \           B          /
 *   B    ==>  / \  ==>   B
 *    \       A   C      /
 *     C                A
 */
static void
zig_zig_left(tree, node)
	sp_tree *tree;
	sp_node *node;
{
	ASSERT(tree != NULL);
	ASSERT(node != NULL);
	ASSERT(node->parent != NULL);
	ASSERT(node->parent->parent != NULL);

	rot_left(tree, node->parent->parent);
	rot_left(tree, node->parent);
}

/*
 * zig_zag_right(T, B)
 *
 * A        A
 *  \        \          B
 *   C  ==>   B   ==>  / \
 *  /          \      A   C
 * B            C
 */
static void
zig_zag_right(tree, node)
	sp_tree *tree;
	sp_node *node;
{
	ASSERT(tree != NULL);
	ASSERT(node != NULL);
	ASSERT(node->parent != NULL);
	ASSERT(node->parent->parent != NULL);

	rot_right(tree, node->parent);
	rot_left(tree, node->parent);
}

/*
 * zig_zag_left(T, B)
 *
 *   C          C
 *  /          /        B
 * A    ==>   B   ==>  / \
 *  \        /        A   C
 *   B      A
 */
static void
zig_zag_left(tree, node)
	sp_tree *tree;
	sp_node *node;
{
	ASSERT(tree != NULL);
	ASSERT(node != NULL);
	ASSERT(node->parent != NULL);
	ASSERT(node->parent->parent != NULL);

	rot_left(tree, node->parent);
	rot_right(tree, node->parent);
}

static void
splay(tree, node)
	sp_tree *tree;
	sp_node *node;
{
	sp_node *parent;

	ASSERT(tree != NULL);
	ASSERT(node != NULL);
	ASSERT(node->parent != NULL);

	parent = node->parent;
	if (parent == tree->root) {
		if (parent->left == node)
			rot_right(tree, parent);
		else
			rot_left(tree, parent);
		return;
	}

	if (parent->left == node) {
		if (parent->parent->left == parent)
			zig_zig_right(tree, node);
		else
			zig_zag_right(tree, node);
	} else {
		if (parent->parent->right == parent)
			zig_zig_left(tree, node);
		else
			zig_zag_left(tree, node);
	}
}

static sp_node *
node_new(key, dat)
	void	*key;
	void	*dat;
{
	sp_node *node;

	node = MALLOC(sizeof(*node));
	if (node == NULL)
		return(NULL);

	node->key = key;
	node->dat = dat;
	node->left = NULL;
	node->right = NULL;
	node->parent = NULL;

	return(node);
}

static sp_node *
node_next(node)
	sp_node *node;
{
	sp_node *temp;

	ASSERT(node != NULL);

	if (node->right) {
		for (node = node->right; node->left; node = node->left)
			/* void */;
	} else {
		temp = node->parent;
		while (temp && temp->right == node) {
			node = temp;
			temp = temp->parent;
		}
		node = temp;
	}
	return(node);
}

static sp_node *
node_prev(node)
	sp_node *node;
{
	sp_node *temp;

	ASSERT(node != NULL);

	if (node->left) {
		for (node = node->left; node->right; node = node->right)
			/* void */;
	} else {
		temp = node->parent;
		while (temp && temp->left == node) {
			node = temp;
			temp = temp->parent;
		}
		node = temp;
	}
	return(node);
}

static sp_node *
node_max(node)
	sp_node *node;
{
	while (node->right)
		node = node->right;
	return(node);
}

static sp_node *
node_min(node)
	sp_node *node;
{
	while (node->left)
		node = node->left;
	return(node);
}

static unsigned
node_height(node)
	const sp_node	*node;
{
	unsigned l, r;

	l = node->left ? node_height(node->left) + 1 : 0;
	r = node->right ? node_height(node->right) + 1 : 0;
	return(MAX(l, r));
}

static unsigned
node_mheight(node)
	const sp_node	*node;
{
	unsigned l, r;

	l = node->left ? node_mheight(node->left) : 0;
	r = node->right ? node_mheight(node->right) : 0;
	return(MIN(l, r));
}

sp_itor *
sp_itor_new(tree)
	sp_tree *tree;
{
	sp_itor *itor;

	ASSERT(tree != NULL);

	itor = MALLOC(sizeof(*itor));
	if (itor == NULL) {
		errno = ENOMEM;
		return(NULL);
	}

	itor->tree = tree;
	sp_itor_first(itor);
	return(itor);
}

dict_itor *
sp_dict_itor_new(tree)
	sp_tree *tree;
{
	dict_itor *itor;

	ASSERT(tree != NULL);

	itor = MALLOC(sizeof(*itor));
	if (itor == NULL) {
		errno = ENOMEM;
		return(NULL);
	}

	itor->itor = sp_itor_new(tree);
	if (!itor->itor) {
		FREE(itor);
		return(NULL);
	}

	itor->destroy = (idestroy_func)sp_itor_destroy;
	itor->valid = (valid_func)sp_itor_valid;
	itor->invalid = (invalidate_func)sp_itor_invalidate;
	itor->next = (next_func)sp_itor_next;
	itor->prev = (prev_func)sp_itor_prev;
	itor->nextn = (nextn_func)sp_itor_nextn;
	itor->prevn = (prevn_func)sp_itor_prevn;
	itor->first = (first_func)sp_itor_first;
	itor->last = (last_func)sp_itor_last;
	itor->key = (key_func)sp_itor_key;
	itor->data = (data_func)sp_itor_data;
	itor->cdata = (cdata_func)sp_itor_cdata;
	itor->dataset = (dataset_func)sp_itor_set_data;
	itor->remove = (iremove_func)sp_itor_remove;

	return(itor);
}

void
sp_itor_destroy(itor)
	sp_itor *itor;
{
	ASSERT(itor != NULL);

	FREE(itor);
}

#define RETVALID(itor)		return(itor->node != NULL)

int
sp_itor_valid(itor)
	const sp_itor *itor;
{
	ASSERT(itor != NULL);

	RETVALID(itor);
}

void
sp_itor_invalidate(itor)
	sp_itor *itor;
{
	ASSERT(itor != NULL);

	itor->node = NULL;
}

int
sp_itor_next(itor)
	sp_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->node == NULL)
		sp_itor_first(itor);
	else
		itor->node = node_next(itor->node);
	RETVALID(itor);
}

int
sp_itor_prev(itor)
	sp_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->node == NULL)
		sp_itor_last(itor);
	else
		itor->node = node_prev(itor->node);
	RETVALID(itor);
}

int
sp_itor_nextn(itor, count)
	sp_itor *itor;
	unsigned count;
{
	ASSERT(itor != NULL);

	if (count) {
		if (itor->node == NULL) {
			sp_itor_first(itor);
			count--;
		}

		while (count-- && itor->node)
			itor->node = node_next(itor->node);
	}

	RETVALID(itor);
}

int
sp_itor_prevn(itor, count)
	sp_itor *itor;
	unsigned count;
{
	ASSERT(itor != NULL);

	if (count) {
		if (itor->node == NULL) {
			sp_itor_last(itor);
			count--;
		}

		while (count-- && itor->node)
			itor->node = node_prev(itor->node);
	}

	RETVALID(itor);
}

int
sp_itor_first(itor)
	sp_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->tree->root == NULL)
		itor->node = NULL;
	else
		itor->node = node_min(itor->tree->root);
	RETVALID(itor);
}

int
sp_itor_last(itor)
	sp_itor *itor;
{
	ASSERT(itor != NULL);

	if (itor->tree->root == NULL)
		itor->node = NULL;
	else
		itor->node = node_max(itor->tree->root);
	RETVALID(itor);
}

const void *
sp_itor_key(itor)
	const sp_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->node ? itor->node->key : NULL);
}

void *
sp_itor_data(itor)
	sp_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->node ? itor->node->dat : NULL);
}

const void *
sp_itor_cdata(itor)
	const sp_itor *itor;
{
	ASSERT(itor != NULL);

	return(itor->node ? itor->node->dat : NULL);
}

int
sp_itor_set_data(itor, dat, del)
	sp_itor	*itor;
	void	*dat;
	int		 del;
{
	ASSERT(itor != NULL);

	if (itor->node == NULL)
		return(-1);

	if (del && itor->tree->dat_del)
		(*itor->tree->dat_del)(itor->node->dat);
	itor->node->dat = dat;
	return(0);
}

int
sp_itor_remove(itor, del)
	sp_itor	*itor;
	int		 del;
{
	sp_tree *tree;
	sp_node *node, *out, *temp;

	ASSERT(itor != NULL);

	tree = itor->tree;
	node = itor->node;
	if (node == NULL)
		return(-1);

	if (!node->left || !node->right) {
		out = node;
		if ((itor->node = node_next(node)) == NULL)
			itor->node = node_prev(itor->node);
	} else {
		for (out = node->right; out->left; out = out->left)
			/* void */;
		SWAP(node->key, out->key, void *);
		SWAP(node->dat, out->dat, void *);
	}

	temp = out->left ? out->left : out->right;
	if (temp)
		temp->parent = out->parent;
	if (out->parent) {
		if (out->parent->left == out) {
			out->parent->left = temp;
		} else {
			ASSERT(out->parent->right == out);
			out->parent->right = temp;
		}
	} else {
		tree->root = temp;
	}

	if (node->left)
		temp = node->left;
	else if (node->right)
		temp = node->right;
	else
		temp = node->parent;
	if (temp)
		while (tree->root != temp)
			splay(tree, temp);

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
