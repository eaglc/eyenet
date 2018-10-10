

#include "eye_core.h"
#include "eye_event.h"
#include "eye_event_timer.h"


eye_event_loop_t *eye_event_loop_init(eye_uint_t ls_n)
{
	eye_pool_t				*pool;
	eye_event_loop_t		*loop;

	pool = eye_create_pool(EYE_LOOP_POOL_SIZE);
	if (pool == NULL) {
		return NULL;
	}

	loop = eye_pcalloc(pool, sizeof(eye_event_loop_t));
	if (loop == NULL) {
		eye_destroy_pool(pool);
		return NULL;
	}

	loop->pool = pool;

	if (eye_event_timer_init(&loop->eye_event_timer_rbtree)
		!= EYE_OK)
	{
		eye_destroy_pool(pool);
		return NULL;
	}

	loop->listening_n = ls_n;
	if(eye_array_init(&loop->listening, pool, loop->listening_n, sizeof(eye_net_listening_t))
		!= EYE_OK) 
	{
		eye_destroy_pool(pool);
		return NULL;
	}

	eye_memzero(loop->listening.elts, loop->listening_n * sizeof(eye_net_listening_t));

	eye_init_events(loop);

	return loop;
}


eye_int_t eye_event_loop_worker_init(eye_event_loop_t *loop, eye_uint_t c_n)
{
	eye_uint_t			 	 i;
	eye_net_connection_t	*c, *next;
	eye_net_listening_t		*ls;
	eye_event_t				*rev, *wev;

	loop->connection_n = c_n;

	loop->connections = eye_alloc(sizeof(eye_net_connection_t) * loop->connection_n);
	if (loop->connections == NULL) {
		// todo:
		return EYE_ERROR;
	}

	loop->read_events = eye_alloc(sizeof(eye_event_t) * loop->connection_n);
	if (loop->read_events == NULL) {
		return EYE_ERROR;
	}

	rev = loop->read_events;
	for (i = 0; i < loop->connection_n; i++) {
		// rev[i].closed = 1;
	}

	loop->write_events = eye_alloc(sizeof(eye_event_t) * loop->connection_n);
	if (loop->write_events == NULL) {
		return EYE_ERROR;
	}

	wev = loop->write_events;
	for (i = 0; i < loop->connection_n; i++) {
		// wev[i].closed = 1;
	}

	i = loop->connection_n;
	c = loop->connections;
	next = NULL;

	do {
		i--;

		c[i].data = next;
		c[i].read = &loop->read_events[i];
		c[i].write = &loop->write_events[i];
		c[i].fd = (eye_socket_t) -1;

		next = &c[i];
	} while (i);

	loop->free_connections = next;
	loop->free_connection_n = loop->connection_n;

	ls = loop->listening.elts;
	for (i = 0; i < loop->listening.nelts; i++) {
		c = eye_get_connection(loop, ls[i].fd);

		// TODO:
		if (c == NULL) {
			return EYE_ERROR;
		}

		c->type = ls[i].type;

		c->listening = &ls[i];
		ls[i].connection = c;

		rev = c->read;
		rev->accept = 1;

		rev->handler = (c->type == SOCK_STREAM) ? eye_net_accept : NULL;

		if (eye_add_event(loop, rev, EYE_EVENT_READ, 0) == EYE_ERROR) {
			return EYE_ERROR;
		}
	}

	return EYE_OK;
}


void eye_event_loop_start(eye_event_loop_t *loop)
{
	eye_msec_t		timer;
	
	loop->running = 1;

	while (loop->running) {
		timer = eye_event_find_timer(&loop->eye_event_timer_rbtree);

		if (timer == EYE_TIMER_INFINITE || timer > 500) {
			timer = 500;
		}
	
		eye_process_events(loop, timer, 0);

		eye_event_expire_timers(&loop->eye_event_timer_rbtree);
	}
}


void eye_event_loop_join(eye_event_loop_t *loop)
{
	eye_pthread_join(loop->pid, NULL);
}


void eye_event_loop_destroy(eye_event_loop_t *loop)
{
	eye_uint_t			i;

	eye_done_events(loop);

	if (loop->connections) {
		
		for (i = 0; i < loop->connection_n; i++) {
			if (loop->connections[i].pool) {
				eye_destroy_pool(loop->connections[i].pool);
			}
		}

		eye_free(loop->connections);
	}

	if (loop->read_events) {
		eye_free(loop->read_events);
	}

	if (loop->write_events) {
		eye_free(loop->write_events);
	}

	eye_destroy_pool(loop->pool);
}

void eye_event_loop_shutdown(eye_event_loop_t *loop)
{
	loop->running = 0;
}