#ifndef __EYE_RBTREE_IMP_H__
#define __EYE_RBTREE_IMP_H__

#include "eye_rbtree.h"


#define eye_rbtree_node_t(K,V)					\
struct {										\
	eye_rbtree_node_t node;						\
	K		key;								\
	V		data;								\
}


typedef eye_rbtree_node_t(eye_int_t, eye_int_t) eye_rbtree_node_int_int_t;
typedef eye_rbtree_node_t(eye_uint_t, eye_uint_t) eye_rbtree_node_uint_uint_t;
typedef eye_rbtree_node_t(uint32_t, uint32_t) eye_rbtree_node_u32_u32_t;
typedef eye_rbtree_node_t(uint64_t, uint64_t) eye_rbtree_node_u64_u64_t;


void inline eye_rbtree_node_insert_int_int(eye_rbtree_node_t *tmp, eye_rbtree_node_t *node, eye_rbtree_node_t *sentinel)
{
	eye_rbtree_node_t  **p;

	for ( ;; ) {

		p = eye_cast(eye_rbtree_node_int_int_t *, node)->key < eye_cast(eye_rbtree_node_int_int_t *, tmp)->key ? &tmp->left : &tmp->right;

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


void inline eye_rbtree_node_insert_uint_uint(eye_rbtree_node_t *tmp, eye_rbtree_node_t *node, eye_rbtree_node_t *sentinel)
{
	eye_rbtree_node_t  **p;

	for ( ;; ) {

		p = eye_cast(eye_rbtree_node_uint_uint_t *, node)->key < eye_cast(eye_rbtree_node_uint_uint_t *, tmp)->key ? &tmp->left : &tmp->right;

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


void inline eye_rbtree_node_insert_u32_u32(eye_rbtree_t *rbt, eye_rbtree_node_t *node)
{
	eye_rbtree_node_t *tmp = rbt->root;
	eye_rbtree_node_t *sentinel = rbt->sentinel;

	eye_rbtree_node_t  **p;

	for ( ;; ) {

		if (eye_cast(eye_rbtree_node_u32_u32_t *, node)->key < eye_cast(eye_rbtree_node_u32_u32_t *, tmp)->key) {
			p = &tmp->left;
		}  else if (eye_cast(eye_rbtree_node_u32_u32_t *, node)->key > eye_cast(eye_rbtree_node_u32_u32_t *, tmp)->key) {
			p = &tmp->right;
		} else {
			eye_rbtree_replace(rbt, tmp, node);
			return;
		}


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

void inline eye_rbtree_node_insert_u64_u64(eye_rbtree_node_t *tmp, eye_rbtree_node_t *node, eye_rbtree_node_t *sentinel)
{
	eye_rbtree_node_t  **p;

	for ( ;; ) {

		p = eye_cast(eye_rbtree_node_u64_u64_t *, node)->key < eye_cast(eye_rbtree_node_u64_u64_t *, tmp)->key ? &tmp->left : &tmp->right;

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


#endif