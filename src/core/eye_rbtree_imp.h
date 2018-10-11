#ifndef __EYE_RBTREE_IMP_H__
#define __EYE_RBTREE_IMP_H__

#include "eye_core.h"

typedef struct {
	eye_rbtree_node_t		node;
	eye_int_t				key;
	eye_int_t				value;
} eye_rbtree_node_int_int_t;

static eye_inline int eye_rbtree_node_int_int_compare(eye_rbtree_node_t *node1, eye_rbtree_t *node2)
{
	eye_int_t ret;

	ret = eye_cast(eye_rbtree_node_int_int_t *, node1)->key - eye_cast(eye_rbtree_node_int_int_t *, node2)->key;

	if (ret > 0) {
		return 1;
	}

	if (ret < 0) {
		return -1;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
	eye_rbtree_node_t	node;
	eye_uint_t			key;
} eye_rbtree_node_timer_t;

static eye_inline int eye_rbtree_node_timer_compare(eye_rbtree_node_t *node1, eye_rbtree_node_t *node2)
{
	eye_int_t ret;

	ret = eye_cast(eye_rbtree_node_timer_t *, node1)->key - eye_cast(eye_rbtree_node_timer_t *, node2)->key;

	if (ret > 0) {
		return 1;
	}

	if (ret < 0) {
		return -1;
	}

	return 0;
}


#endif