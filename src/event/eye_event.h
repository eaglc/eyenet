#ifndef __EYE_EVENT_H__
#define __EYE_EVENT_H__


#include "eye_core.h"

// events
#define EYE_EVENT_READ		(1u << 0)
#define EYE_EVENT_WRITE		(1u << 1)

// corresponding to event module flags
#define EYE_EVENT_CLEAR		(1u << 2)
#define EYE_EVENT_ONESHOT	(1u << 3)
#define EYE_EVENT_LEVEL		(1u << 4)

// events ext flags
#define EYE_EVENT_CLOSE		(1u << 5)


// flags
/*
 * The event filter requires to read/write the whole data:
 * select, poll, /dev/poll, kqueue, epoll.
 */
#define EYE_USE_LEVEL_EVENT      0x00000001

/*
 * The event filter is deleted after a notification without an additional
 * syscall: kqueue, epoll.
 */
#define EYE_USE_ONESHOT_EVENT    0x00000002

/*
 * The event filter notifies only the changes and an initial level:
 * kqueue, epoll.
 */
#define EYE_USE_CLEAR_EVENT      0x00000004


typedef struct eye_event_actions_s  eye_event_actions_t;

typedef void (*eye_event_handler)(eye_event_t *ev);


struct eye_event_s {
	void 				*data;			// pointer to eye_net_connection_t
	unsigned int 	 	active:1;
	eye_event_handler 	handler;

    unsigned         	ready:1;
    unsigned            eof:1;
	unsigned         	pending_eof:1;
    unsigned         	available:1;
    unsigned			error:1;
    unsigned			write:1;
    unsigned			timedout:1;
    unsigned			timer_set:1;
    unsigned			accept:1;
    unsigned			cancelable:1;

    eye_rbtree_node_t	timer;
};


struct eye_event_actions_s {
	eye_int_t  (*enable)(eye_event_loop_t *loop, eye_event_t *ev, eye_int_t event, eye_uint_t flags);
	eye_int_t  (*disable)(eye_event_loop_t *loop, eye_event_t *ev, eye_int_t event, eye_uint_t flags);

	eye_int_t  (*add)(eye_event_loop_t *loop, eye_event_t *ev, eye_int_t event, eye_uint_t flags);
	eye_int_t  (*del)(eye_event_loop_t *loop, eye_event_t *ev, eye_int_t event, eye_uint_t flags);

	eye_int_t  (*add_conn)(eye_event_loop_t *loop, eye_net_connection_t *c);
	eye_int_t  (*del_conn)(eye_event_loop_t *loop, eye_net_connection_t *c, eye_uint_t flags);

	eye_int_t  (*process)(eye_event_loop_t *loop, eye_msec_t timer, eye_uint_t flags);

	eye_int_t  (*init)(eye_event_loop_t *loop);
	eye_int_t  (*done)(eye_event_loop_t *loop);
};


extern struct eye_event_actions_s eye_event_actions;


#define eye_process_events		eye_event_actions.process
#define eye_done_events			eye_event_actions.done
#define eye_init_events			eye_event_actions.init

#define eye_add_event        	eye_event_actions.add
#define eye_del_event        	eye_event_actions.del
#define eye_enable_event		eye_event_actions.enable
#define eye_disable_event		eye_event_actions.disable
#define eye_add_conn         	eye_event_actions.add_conn
#define eye_del_conn         	eye_event_actions.del_conn

extern eye_uint_t             	eye_event_flags;

extern eye_os_io_t	eye_io;

#define eye_recv				eye_io.recv
#define eye_send				eye_io.send
#define eye_udp_recv			eye_io.udp_recv
#define eye_udp_send			eye_io.udp_send

eye_int_t eye_handle_read_event(eye_event_loop_t *loop, eye_event_t *rev, eye_uint_t flags);
eye_int_t eye_handle_write_event(eye_event_loop_t *loop, eye_event_t *wev);

#endif