

#include "eye_core.h"
#include "eye_event.h"


static void eye_close_accepted_connection(eye_net_connection_t *c);


void eye_net_accept(eye_event_t *ev)
{
	eye_socket_t				 s;
	eye_sockaddr_t				 sa;
	socklen_t					 socklen;
	eye_net_listening_t			*ls;
	eye_net_connection_t		*lc, *c;
	eye_event_t					*rev, *wev;
	eye_err_t					 err;

	lc = ev->data;
	ls = lc->listening;

	ev->ready = 0;

	do {
		socklen = sizeof(eye_sockaddr_t);

		s = accept(ls->fd, &sa.sockaddr, &socklen);

		if (s == (eye_socket_t) -1) {
			err = eye_socket_errno;

			if (err == EYE_EAGAIN) {
				// log
				return;
			}

			// log

			if (err == EYE_ECONNABORTED) {

				if (ev->available) {
					continue;
				}
			}
		}

		c = eye_get_connection(ls->loop, s);

		if (c == NULL) {
			if (eye_close_socket(s) == -1) {
				// log
			}

			return;
		}

		c->type = SOCK_STREAM;

		c->pool = eye_create_pool(ls->pool_size);
		if (c->pool == NULL) {
			eye_close_accepted_connection(c);
			return;
		}

		if (socklen > (socklen_t) sizeof(eye_sockaddr_t)) {
			socklen = sizeof(eye_sockaddr_t);
		}

		c->sockaddr = eye_palloc(c->pool, socklen);
		if (c->sockaddr == NULL) {
			eye_close_accepted_connection(c);
			return;
		}

		eye_memcpy(c->sockaddr, &sa, socklen);

		if (eye_nonblocking(s) == -1) {
			// log
			eye_close_accepted_connection(c);
			return;
		}

		c->recv = eye_recv;
		c->send = eye_send;

		c->socklen = socklen;
		c->listening = ls;
		c->local_sockaddr = ls->sockaddr;
		c->local_socklen = ls->socklen;

		rev = c->read;
		wev = c->write;

		wev->ready = 1;

		if (ls->addr_ntop) {
			c->addr_text.data = eye_pnalloc(c->pool, ls->addr_text_max_len);
			if (c->addr_text.data == NULL) {
				eye_close_accepted_connection(c);
				return;
			}

			c->addr_text.len = eye_sock_ntop(c->sockaddr, c->socklen,
											 c->addr_text.data,
											 ls->addr_text_max_len, 0);
			if (c->addr_text.len == 0) {
				eye_close_accepted_connection(c);
				return;
			}
		}

		ls->handler(c);

	} while (ev->available);
}


static void eye_close_accepted_connection(eye_net_connection_t *c)
{
	eye_socket_t	fd;

	eye_free_connection(c);

	fd = c->fd;
	c->fd = (eye_socket_t) -1;

	if (eye_close_socket(fd) == -1) {
		// log
	}

	if (c->pool) {
		eye_destroy_pool(c->pool);
	}
}
