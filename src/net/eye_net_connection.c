

#include "eye_core.h"
#include "eye_event.h"
#include "eye_event_timer.h"


eye_int_t eye_net_connection_error(eye_net_connection_t *c, eye_err_t err, char *text)
{
	// TODO:
	return EYE_ERROR;
}

eye_net_connection_t * eye_get_connection(eye_event_loop_t *loop, eye_socket_t s)
{
	eye_net_connection_t 	*c;
	eye_event_t       		*rev, *wev;

	c = loop->free_connections;

	if (c == NULL) {
		// log;
		return NULL;
	}

	loop->free_connections = c->data;
	loop->free_connection_n--;

	rev = c->read;
	wev = c->write;

	eye_memzero(c, sizeof(eye_net_connection_t));

	c->read = rev;
	c->write = wev;
	c->fd = s;

	eye_memzero(rev, sizeof(eye_event_t));
	eye_memzero(wev, sizeof(eye_event_t));

	rev->data = c;
	wev->data = c;

	wev->write = 1;

	return c;
}

void eye_free_connection(eye_net_connection_t *c)
{
	eye_event_loop_t	*loop;

	loop = c->listening->loop;

	c->data = loop->free_connections;
	loop->free_connections = c;
	loop->free_connection_n++;
}

void eye_close_connection(eye_net_connection_t *c)
{
	// todo:
	eye_event_loop_t	*loop;
	eye_err_t			 err;
	eye_socket_t		 fd;

	loop = c->listening->loop;

	if (c->fd == (eye_socket_t) -1) {
		// log
		return;
	}

	if (c->read->timer_set) {
		eye_del_timer(&loop->eye_event_timer_rbtree, c->read);
	}

	if (c->write->timer_set) {
		eye_del_timer(&loop->eye_event_timer_rbtree, c->write);
	}

	if (!c->shared) {
		if (eye_del_conn) {
			eye_del_conn(loop, c, EYE_EVENT_CLOSE);
		} else {
			if (c->read->active) {
				eye_del_event(loop, c->read, EYE_EVENT_READ, EYE_EVENT_CLOSE);
			}

			if (c->write->active) {
				eye_del_event(loop, c->write, EYE_EVENT_WRITE, EYE_EVENT_CLOSE);
			}
		}
	}

	//c->read->closed = 1;
	//c->write->closed = 1;


	eye_free_connection(c);

	fd = c->fd;
	c->fd = (eye_socket_t) -1;

	if (c->shared) {
		return;
	}

	if (eye_close_socket(fd) == -1) {
		err = eye_socket_errno;
	}
}