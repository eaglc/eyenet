#ifndef __EYE_RBTREE_H__
#define __EYE_RBTREE_H__


#include "eye_core.h"


#define  eye_cast(type, p)	((type)(p))

typedef struct eye_rbtree_node_s	eye_rbtree_node_t;

struct eye_rbtree_node_s {
	eye_rbtree_node_t	*left;
	eye_rbtree_node_t	*right;
	eye_rbtree_node_t	*parent;
	unsigned int		 color:1;
};

typedef struct eye_rbtree_s	eye_rbtree_t;

typedef void (*eye_rbtree_insert_pt) (eye_rbtree_t *rbt, eye_rbtree_node_t *node);
typedef int (*eye_rbtree_compare_pt) (eye_rbtree_node_t *node1, eye_rbtree_node_t *node2);

struct eye_rbtree_s {
	eye_rbtree_node_t			*root;
	eye_rbtree_node_t			*sentinel;
	eye_rbtree_insert_pt	 	 insert;
	eye_rbtree_compare_pt		 compare;
};

#define eye_rbtree_init(tree, s, i, c)											\
	eye_rbtree_sentinel_init(s);                                              	\
	(tree)->root = s;                                                         	\
	(tree)->sentinel = s;                                                     	\
	(tree)->insert = i;															\
	(tree)->compare = c;


void eye_rbtree_insert(eye_rbtree_t *tree, eye_rbtree_node_t *node);
void eye_rbtree_delete(eye_rbtree_t *tree, eye_rbtree_node_t *node);
eye_rbtree_node_t *eye_rbtree_next(eye_rbtree_t *tree,
								   eye_rbtree_node_t *node);

#define eye_rbt_red(node)               ((node)->color = 1)
#define eye_rbt_black(node)             ((node)->color = 0)
#define eye_rbt_is_red(node)            ((node)->color)
#define eye_rbt_is_black(node)          (!eye_rbt_is_red(node))
#define eye_rbt_copy_color(n1, n2)      (n1->color = n2->color)

#define eye_rbtree_sentinel_init(node)  { eye_rbt_black(node); }


static eye_inline eye_rbtree_node_t *
	eye_rbtree_min(eye_rbtree_node_t *node, eye_rbtree_node_t *sentinel)
{
	while (node->left != sentinel) {
		node = node->left;
	}

	return node;
}

static eye_inline void
	eye_rbtree_replace(eye_rbtree_t *rbt, eye_rbtree_node_t *old, eye_rbtree_node_t *newnode)
{
	eye_rbtree_node_t *parent = old->parent;

	if (parent == NULL) {
		newnode->parent = NULL;
		newnode->left = rbt->sentinel;
		newnode->right = rbt->sentinel;
		eye_rbt_black(newnode);
		rbt->root = newnode;
		return;
	}

	if (parent->left == old) {
		parent->left = newnode;
	} else {
		parent->right = newnode;
	}

	newnode->left = old->left;
	newnode->right = old->right;

	old->left->parent = newnode;
	old->right->parent = newnode;
	newnode->color = old->color;

	newnode->parent = parent;
}

static eye_inline void 
	eye_default_rbtree_insert_value(eye_rbtree_t *rbt, eye_rbtree_node_t *node)
{
	eye_rbtree_node_t  **root, *sentinel;
	eye_rbtree_node_t	**p;
	eye_rbtree_node_t	*tmp;

    root = &rbt->root;
    sentinel = rbt->sentinel;

    tmp = *root;

	for ( ;; ) {

		p = rbt->compare(tmp, node) < 0 ? &tmp->left : &tmp->right;

		if (*p == sentinel) {
			break;
		}

		tmp = *p;
	}

	*p = node;
	node->parent = tmp;
	node->left = sentinel;
	node->right = sentinel;
	eye_rbt_red(node);
}


// todo: 遍历以及查找

#endif