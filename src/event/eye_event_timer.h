#ifndef __EYE_EVENT_TIMER_H__
#define __EYE_EVENT_TIMER_H__

#include "eye_core.h"
#include "eye_event.h"

#define EYE_TIMER_INFINITE		(eye_msec_t) -1

#define EYE_TIMER_LAZY_DELAY	300


eye_int_t eye_event_timer_init(eye_rbtree_t *rbt);
eye_msec_t eye_event_find_timer(eye_rbtree_t *rbt);
void eye_event_expire_timers(eye_rbtree_t *rbt);
eye_int_t eye_event_no_timers_left(eye_rbtree_t *rbt);


static eye_inline void eye_event_del_timer(eye_rbtree_t *rtb, eye_event_t *ev)
{
	eye_rbtree_delete(rtb, eye_cast(eye_rbtree_node_t *, &ev->timer));

	ev->timer_set = 0;
}

static eye_inline void eye_event_add_timer(eye_rbtree_t *rbt, eye_event_t *ev, eye_msec_t timer)
{
	eye_msec_t			eye_current_msec;
	eye_msec_t			key;
	eye_msec_int_t		diff;

	eye_current_msec = eye_monotonic_time(0, 0);

	key = eye_current_msec + timer;

	if (ev->timer_set) {
		diff = (eye_msec_int_t) (key - ev->timer.key);

		if (eye_abs(diff) < EYE_TIMER_LAZY_DELAY) {
			return;
		}

		eye_del_timer(rbt, ev);
	}

	ev->timer.key = key;

	eye_rbtree_insert(rbt, eye_cast(eye_rbtree_node_t *, &ev->timer));

	ev->timer_set = 1;
}


#endif /*__EYE_EVENT_TIMER_H__*/