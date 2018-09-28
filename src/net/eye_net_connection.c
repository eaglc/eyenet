

#include "eye_core.h"
#include "eye_event.h"

eye_int_t eye_net_connection_error(eye_net_connection_t *c, eye_err_t err, char *text)
{
	return 0;
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
}