

#ifndef __EYE_EVENT_LOOP_H__
#define __EYE_EVENT_LOOP_H__


#include "eye_core.h"
#include "eye_event.h"

#ifndef EYE_LOOP_POOL_SIZE
#define EYE_LOOP_POOL_SIZE		EYE_DEFAULT_POOL_SIZE
#endif


struct eye_event_loop_s {
	void					*module_ctx;

	eye_array_t				 listening;
	eye_uint_t				 listening_n;

	eye_net_connection_t	*free_connections;
	eye_uint_t				 free_connection_n;
	eye_uint_t				 connection_n;
	eye_net_connection_t	*connections;
	eye_event_t				*read_events;
	eye_event_t				*write_events;
	
	eye_pool_t				*pool;
	eye_pthread_t			 pid;

	// timers
	eye_rbtree_t			eye_event_timer_rbtree;

	volatile int			running;
};


eye_event_loop_t *eye_event_loop_init(eye_uint_t ls_n);
eye_int_t eye_event_loop_worker_init(eye_event_loop_t *loop, eye_uint_t c_n);
void eye_event_loop_start(eye_event_loop_t *loop);
void eye_event_loop_start_thread(eye_event_loop_t *loop);
void eye_event_loop_join(eye_event_loop_t *loop);
void eye_event_loop_destroy(eye_event_loop_t *loop);
void eye_event_loop_shutdown(eye_event_loop_t *loop);


#endif