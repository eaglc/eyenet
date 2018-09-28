#ifndef __EYE_EVENT_TIMER_H__
#define __EYE_EVENT_TIMER_H__


#include "eye_core.h"


#define EYE_TIMER_INFINITE		(eye_msec_t) -1

#define EYE_TIMER_LAZY_DELAY	300


typedef struct {
	eye_rbtree_node_t	node;
	eye_uint_t			key;
} eye_rbtree_node_timer_t;


eye_int_t eye_event_timer_init(eye_rbtree_t *rbt);
eye_msec_t eye_event_find_timer(eye_rbtree_t *rbt);
void eye_event_expire_timers(eye_rbtree_t *rbt);
eye_int_t eye_event_no_timers_left(eye_rbtree_t *rbt);


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


#endif /*__EYE_EVENT_TIMER_H__*/