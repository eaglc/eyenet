#ifndef __EYE_EVENT_LOOP_H__
#define __EYE_EVENT_LOOP_H__


#include "eye_core.h"


typedef eye_event_loop_s  eye_event_loop_t;


struct eye_event_loop_s {
	void 				   *module_ctx;
	eye_event_actions_t		actions;
};


eye_event_loop_init(eye_event_loop_t *p);
eye_event_loop_start(eye_event_loop_t *p);
eye_event_loop_destroy(eye_event_loop_t *p);


#endif