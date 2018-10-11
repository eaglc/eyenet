

#include "eye_event_timer.h"
#include "eye_event.h"

static eye_rbtree_node_t	eye_event_timer_sentinel;

eye_int_t eye_event_timer_init(eye_rbtree_t *rbt)
{
	eye_rbtree_init(rbt, &eye_event_timer_sentinel, 
		eye_default_rbtree_insert_value, eye_rbtree_node_timer_compare);

	return EYE_OK;
}

eye_msec_t eye_event_find_timer(eye_rbtree_t *rbt)
{
	eye_msec_t			 eye_current_msec;
	eye_msec_int_t		 timer;
	eye_rbtree_node_t	*node, *root, *sentinel;

	if (rbt->root == &eye_event_timer_sentinel) {
		return EYE_TIMER_INFINITE;
	}

	root = rbt->root;
	sentinel = rbt->sentinel;

	node = eye_rbtree_min(root, sentinel);

	eye_current_msec = eye_monotonic_time(0, 0);
	timer = (eye_msec_int_t) (eye_cast(eye_rbtree_node_timer_t *, node)->key - eye_current_msec);

	return (eye_msec_t) (timer > 0 ? timer : 0);
}

void eye_event_expire_timers(eye_rbtree_t *rbt)
{
	eye_event_t			*ev;
	eye_rbtree_node_t	*node, *root, *sentinel;
	eye_msec_t			 eye_current_msec;

	sentinel = rbt->sentinel;
	eye_current_msec = eye_monotonic_time(0, 0);

	for ( ;; ) {
		root = rbt->root;

		if (root == sentinel) {
			return;
		}

		node = eye_rbtree_min(root, sentinel);

		if ((eye_msec_int_t) (eye_cast(eye_rbtree_node_timer_t *, node)->key - eye_current_msec)
			> 0) {
			return;
		}

		ev = (eye_event_t *)((char *) node - offsetof(eye_event_t, timer));

		eye_rbtree_delete(rbt, eye_cast(eye_rbtree_node_t *, &ev->timer));

		ev->timer_set = 0;
		ev->timedout = 1;

		ev->handler(ev);
	}
}

eye_int_t eye_event_no_timers_left(eye_rbtree_t *rbt)
{
	eye_event_t			*ev;
	eye_rbtree_node_t	*node, *root, *sentinel;

	sentinel = rbt->sentinel;
	root = rbt->root;

	if (root == sentinel) {
		return EYE_OK;
	}

	for (node = eye_rbtree_min(root, sentinel);
		node; 
		node = eye_rbtree_next(rbt, node))
	{
		ev = (eye_event_t *) ((char *) node - offsetof(eye_event_t, timer));

		if (!ev->cancelable) {
			return EYE_AGAIN;
		}
	}

	return EYE_OK;
}