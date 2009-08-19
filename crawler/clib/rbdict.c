#include <stdlib.h>
#include <errno.h>

#include "rbdict.h"

#define NDEBUG

#define RB_RED		0
#define RB_BLACK	1

typedef struct __RbNode TRbNode;
struct __RbNode {
	void    *key;
	void    *dat;
	TRbNode *left;
	TRbNode *right;
	TRbNode *parent;
	unsigned color:1;
};

struct __RbDict {
	TRbNode *root;
	unsigned count;
	DICT_CMP_FUNC key_cmp;
	DICT_DEL_FUNC key_del;
	DICT_DEL_FUNC dat_del;
};

struct __RbIterator {
	TRbDict *dict;
	TRbNode *node;
};

static TRbNode nil = { NULL, NULL, NULL, NULL, NULL, RB_BLACK };
#define RB_NIL  &nil

static void rot_left(TRbDict *dict, TRbNode *node);
static void rot_right(TRbDict *dict, TRbNode *node);
static void insert_fixup(TRbDict *dict, TRbNode *node);
static void delete_fixup(TRbDict *dict, TRbNode *node);
static unsigned node_height(const TRbNode *node);
static unsigned node_mheight(const TRbNode *node);
static TRbNode *node_new(void *key, void *dat);
static TRbNode *node_next(TRbNode *node);
static TRbNode *node_prev(TRbNode *node);
static TRbNode *node_max(TRbNode *node);
static TRbNode *node_min(TRbNode *node);

#ifndef NDEBUG
#define ASSERT(expr)    assert(expr)
#else
#define ASSERT(expr)
#endif

#define MIN(a,b)    ((a) < (b) ? (a) : (b))
#define MAX(a,b)    ((a) > (b) ? (a) : (b))
#define SWAP(a,b,t) { t _t = (a); (a) = (b); (b) = _t; }

TRbDict* rb_dict_new(DICT_CMP_FUNC key_cmp, DICT_DEL_FUNC key_del, DICT_DEL_FUNC dat_del)
{
	TRbDict *dict;

	ASSERT(key_cmp != NULL);

	dict = malloc(sizeof(*dict));
	if (dict == NULL) {
		errno = ENOMEM;
		return(NULL);
	}

	dict->root = RB_NIL;
	dict->count = 0;
	dict->key_cmp = key_cmp;
	dict->key_del = key_del;
	dict->dat_del = dat_del;

	return(dict);
}

void rb_dict_destroy(TRbDict *dict, int del)
{
	ASSERT(dict != NULL);

	if (dict->root != RB_NIL)
		rb_dict_empty(dict, del);

	free(dict);
}

int rb_dict_search(TRbDict *dict, const void *key, void **dat)
{
	int rv;
	TRbNode *node;

	ASSERT(dict != NULL);

	rv = -1;
	node = dict->root;
	while (node != RB_NIL) {
		rv = (*dict->key_cmp)(key, node->key);
		if (rv == 0)
			break;
		node = rv < 0 ? node->left : node->right;
	}

	if (rv == 0)
	{
		*dat = node->dat;
	}

	return rv;
}


int rb_dict_insert(TRbDict *dict, void *key, void *dat, int overwrite)
{
	int rv = 0;
	TRbNode *node, *parent = RB_NIL;

	ASSERT(dict != NULL);

	node = dict->root;

	while (node != RB_NIL) {
		rv = (*dict->key_cmp)(key, node->key);
		if (rv == 0) {
			if (overwrite == 0)
				return(1);
			if (dict->key_del)
				(*dict->key_del)(node->key);
			if (dict->dat_del && node->dat)
				(*dict->dat_del)(node->dat);
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
		dict->root = node;
	} else {
		if (rv < 0)
			parent->left = node;
		else
			parent->right = node;
	}
	insert_fixup(dict, node);

	dict->count++;
	return(0);
}

int rb_dict_probe(TRbDict *dict, void *key, void **dat)
{
	int rv = 0;
	TRbNode *node, *parent = RB_NIL;

	ASSERT(dict != NULL);

	node = dict->root;

	while (node != RB_NIL) {
		rv = (*dict->key_cmp)(key, node->key);
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
		dict->root = node;
	} else {
		if (rv < 0)
			parent->left = node;
		else
			parent->right = node;
	}

	insert_fixup(dict, node);
	dict->count++;
	return(1);
}

void insert_fixup(TRbDict *dict, TRbNode *node)
{
	TRbNode *temp;

	ASSERT(dict != NULL);
	ASSERT(node != NULL);

	while (node != dict->root && node->parent->color == RB_RED) {
		if (node->parent == node->parent->parent->left) {
			temp = node->parent->parent->right;
			if (temp->color == RB_RED) {
				temp->color = RB_BLACK;
				node->parent->color = RB_BLACK;
				node->parent->parent->color = RB_RED;
				node = node->parent->parent;
			} else {
				if (node == node->parent->right) {
					node = node->parent;
					rot_left(dict, node);
				}
				node->parent->color = RB_BLACK;
				node->parent->parent->color = RB_RED;
				rot_right(dict, node->parent->parent);
			}
		} else {
			temp = node->parent->parent->left;
			if (temp->color == RB_RED) {
				temp->color = RB_BLACK;
				node->parent->color = RB_BLACK;
				node->parent->parent->color = RB_RED;
				node = node->parent->parent;
			} else {
				if (node == node->parent->left) {
					node = node->parent;
					rot_right(dict, node);
				}
				node->parent->color = RB_BLACK;
				node->parent->parent->color = RB_RED;
				rot_left(dict, node->parent->parent);
			}
		}
	}

	dict->root->color = RB_BLACK;
}

int rb_dict_remove(TRbDict *dict, const void *key, int del)
{
	int rv;
	TRbNode *node, *temp, *out;

	ASSERT(dict != NULL);

	node = dict->root;
	while (node != RB_NIL) {
		rv = (*dict->key_cmp)(key, node->key);
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
		dict->root = temp;
	}

	if (out->color == RB_BLACK)
		delete_fixup(dict, temp);
	if (del) {
		if (dict->key_del)
			(*dict->key_del)(out->key);
		if (dict->dat_del && out->dat)
			(*dict->dat_del)(out->dat);
	}
	free(out);

	dict->count--;

	return(0);
}

static void delete_fixup(TRbDict *dict, TRbNode *node)
{
	TRbNode *temp;

	ASSERT(dict != NULL);
	ASSERT(node != NULL);

	while (node != dict->root && node->color == RB_BLACK) {
		if (node->parent->left == node) {
			temp = node->parent->right;
			if (temp->color == RB_RED) {
				temp->color = RB_BLACK;
				node->parent->color = RB_RED;
				rot_left(dict, node->parent);
				temp = node->parent->right;
			}
			if (temp->left->color == RB_BLACK && temp->right->color == RB_BLACK) {
				temp->color = RB_RED;
				node = node->parent;
			} else {
				if (temp->right->color == RB_BLACK) {
					temp->left->color = RB_BLACK;
					temp->color = RB_RED;
					rot_right(dict, temp);
					temp = node->parent->right;
				}
				temp->color = node->parent->color;
				temp->right->color = RB_BLACK;
				node->parent->color = RB_BLACK;
				rot_left(dict, node->parent);
				node = dict->root;
			}
		} else {
			temp = node->parent->left;
			if (temp->color == RB_RED) {
				temp->color = RB_BLACK;
				node->parent->color = RB_RED;
				rot_right(dict, node->parent);
				temp = node->parent->left;
			}
			if (temp->right->color == RB_BLACK && temp->left->color == RB_BLACK) {
				temp->color = RB_RED;
				node = node->parent;
			} else {
				if (temp->left->color == RB_BLACK) {
					temp->right->color = RB_BLACK;
					temp->color = RB_RED;
					rot_left(dict, temp);
					temp = node->parent->left;
				}
				temp->color = node->parent->color;
				node->parent->color = RB_BLACK;
				temp->left->color = RB_BLACK;
				rot_right(dict, node->parent);
				node = dict->root;
			}
		}
	}

	node->color = RB_BLACK;
}

void rb_dict_empty(TRbDict *dict, int del)
{
	TRbNode *node, *parent;

	ASSERT(dict != NULL);

	node = dict->root;
	while (node != RB_NIL) {
		parent = node->parent;
		if (node->left && node->left != RB_NIL) {
			node = node->left;
			continue;
		}
		if (node->right && node->right != RB_NIL) {
			node = node->right;
			continue;
		}

		if (del) {
			if (dict->key_del)
				(*dict->key_del)(node->key);
			if (dict->dat_del && node->dat)
				(*dict->dat_del)(node->dat);
		}
		free(node);
		dict->count--;

		if (parent) {
			if (parent->left == node)
				parent->left = NULL;
			else
				parent->right = NULL;
		}
		node = parent;
	}

	dict->root = NULL;
	ASSERT(dict->count == 0);
}

unsigned rb_dict_count(const TRbDict *dict)
{
	ASSERT(dict != NULL);

	return(dict->count);
}

unsigned rb_dict_height(const TRbDict *dict)
{
	ASSERT(dict != NULL);

	return(dict->root != RB_NIL ? node_height(dict->root) : 0);
}

unsigned rb_dict_mheight(const TRbDict *dict)
{
	ASSERT(dict != NULL);

	return(dict->root != RB_NIL ? node_mheight(dict->root) : 0);
}

const void *rb_dict_min(const TRbDict *dict)
{
	const TRbNode *node;

	ASSERT(dict != NULL);

	if (dict->root == RB_NIL)
		return(NULL);

	for (node = dict->root; node->left; node = node->left)
		/* void */;
	return(node->key);
}

const void *rb_dict_max(const TRbDict *dict)
{
	const TRbNode *node;

	ASSERT(dict != NULL);

	if (dict->root == RB_NIL)
		return(NULL);

	for (node = dict->root; node->right; node = node->right)
		/* void */;
	return(node->key);
}

void rb_dict_walk(TRbDict *dict, DICT_VIS_FUNC visit, void *args)
{
	TRbNode *node;

	ASSERT(dict != NULL);
	ASSERT(visit != NULL);

	if (dict->root == RB_NIL)
		return;

	for (node = node_min(dict->root); node != RB_NIL; node = node_next(node))
		if ((*visit)(node->key, node->dat, args) != 0)
			break;
}

static unsigned node_height(const TRbNode *node)
{
	unsigned l, r;

	l = node->left != RB_NIL ? node_height(node->left) + 1 : 0;
	r = node->right != RB_NIL ? node_height(node->right) + 1 : 0;
	return(MAX(l, r));
}

static unsigned node_mheight(const TRbNode *node)
{
	unsigned l, r;

	l = node->left != RB_NIL ? node_mheight(node->left) + 1 : 0;
	r = node->right != RB_NIL ? node_mheight(node->right) + 1 : 0;
	return(MIN(l, r));
}

static void rot_left(TRbDict *dict, TRbNode *node)
{
	TRbNode *right;

	ASSERT(dict != NULL);
	ASSERT(node != NULL);

	right = node->right;
	node->right = right->left;
	if (right->left != RB_NIL)
		right->left->parent = node;
	right->parent = node->parent;
	if (node->parent == RB_NIL)
		dict->root = right;
	else {
		if (node->parent->left == node)
			node->parent->left = right;
		else
			node->parent->right = right;
	}
	right->left = node;
	node->parent = right;
}

static void rot_right(TRbDict *dict, TRbNode *node)
{
	TRbNode *left;

	ASSERT(dict != NULL);
	ASSERT(node != NULL);

	left = node->left;
	node->left = left->right;
	if (left->right != RB_NIL)
		left->right->parent = node;
	left->parent = node->parent;
	if (node->parent == RB_NIL)
		dict->root = left;
	else {
		if (node->parent->left == node)
			node->parent->left = left;
		else
			node->parent->right = left;
	}
	left->right = node;
	node->parent = left;
}

static TRbNode *node_new(void *key, void *dat)
{
	TRbNode *node;

	node = malloc(sizeof(*node));
	if (node == NULL)
		return(NULL);

	node->key = key;
	node->dat = dat;
	node->color = RB_RED;
	node->left = RB_NIL;
	node->right = RB_NIL;

	return(node);
}

static TRbNode *node_next(TRbNode *node)
{
	TRbNode *temp;

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

static TRbNode *node_prev(TRbNode *node)
{
	TRbNode *temp;

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

static TRbNode *node_max(TRbNode *node)
{
	ASSERT(node != NULL);

	while (node->right != RB_NIL)
		node = node->right;
	return(node);
}

static TRbNode *node_min(TRbNode *node)
{
	ASSERT(node != NULL);

	while (node->left != RB_NIL)
		node = node->left;
	return(node);
}

TRbIterator *rb_itor_new(TRbDict *dict)
{
	TRbIterator *itor;

	ASSERT(dict != NULL);

	itor = malloc(sizeof(*itor));
	if (itor == NULL)
		return(NULL);

	itor->dict = dict;
	rb_itor_first(itor);
	return(itor);
}

void rb_itor_destroy(TRbIterator *itor)
{
	ASSERT(itor != NULL);

	free(itor);
}

#define RETVALID(itor)      return(itor->node != RB_NIL)

int rb_itor_valid(const TRbIterator *itor)
{
	ASSERT(itor != NULL);

	RETVALID(itor);
}

void rb_itor_invalidate(TRbIterator *itor)
{
	ASSERT(itor != NULL);

	itor->node = RB_NIL;
}

int rb_itor_next(TRbIterator *itor)
{
	ASSERT(itor != NULL);

	if (itor->node == RB_NIL)
		rb_itor_first(itor);
	else
		itor->node = node_next(itor->node);
	RETVALID(itor);
}

int rb_itor_prev(TRbIterator *itor)
{
	ASSERT(itor != NULL);

	if (itor->node == RB_NIL)
		rb_itor_last(itor);
	else
		itor->node = node_prev(itor->node);
	RETVALID(itor);
}

int rb_itor_nextn(TRbIterator *itor, unsigned count)
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

int rb_itor_prevn(TRbIterator *itor, unsigned count)
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

int rb_itor_first(TRbIterator *itor)
{
	ASSERT(itor != NULL);

	if (itor->dict->root == RB_NIL)
		itor->node = RB_NIL;
	else
		itor->node = node_min(itor->dict->root);
	RETVALID(itor);
}

int rb_itor_last(TRbIterator *itor)
{
	ASSERT(itor != NULL);

	if (itor->dict->root == RB_NIL)
		itor->node = RB_NIL;
	else
		itor->node = node_max(itor->dict->root);
	RETVALID(itor);
}

const void *rb_itor_key(const TRbIterator *itor)
{
	ASSERT(itor != NULL);

	return(itor->node != RB_NIL ? itor->node->key : NULL);
}

void *rb_itor_data(TRbIterator *itor)
{
	ASSERT(itor != NULL);

	return(itor->node != RB_NIL ? itor->node->dat : NULL);
}

const void *rb_itor_cdata(const TRbIterator *itor)
{
	ASSERT(itor != NULL);

	return(itor->node != RB_NIL ? itor->node->dat : NULL);
}

