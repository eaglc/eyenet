#ifndef __EYE_EVENT_H__
#define __EYE_EVENT_H__


#include "eye_core.h"


#define EYE_EVENT_READ		(0x01 << 0)
#define EYE_EVENT_WRITE		(0x01 << 1)


typedef eye_event_s  eye_event_t;
typedef eye_event_actions_s  eye_event_actions_t;

typedef void (*eye_event_handler)(eye_event_t *ev);


struct eye_event_s {
	void 				*data;			// pointer to carrier
	unsigned int 	 	active:1;
	eye_event_handler 	handler;
};


struct eye_event_actions_s {
	eye_int_t  (*enable)(eye_event_loop_t *loop, eye_event_t *ev, eye_int_t event);
	eye_int_t  (*disable)(eye_event_loop_t *loop, eye_event_t *ev, eye_int_t event);
	eye_int_t  (*add)(eye_event_loop_t *loop, eye_event_t *ev, eye_int_t event);
	eye_int_t  (*del)(eye_event_loop_t *loop, eye_event_t *ev, eye_int_t event);
	eye_int_t  (*process)(eye_event_loop_t *loop, eye_msec_t timer);
	eye_int_t  (*init)(eye_event_loop_t *loop);
	eye_int_t  (*done)(eye_event_loop_t *loop);
};


#endif