

#include "eye_core.h"


eye_net_listening_t *eye_create_listening(eye_event_loop_t *loop, struct sockaddr *sockaddr, socklen_t socklen)
{
	size_t					 len;
	eye_net_listening_t		*ls;
	struct sockaddr			*sa;
	u_char					 text[EYE_SOCKADDR_STRLEN];

	ls = eye_array_push(&loop->listening);
	if (ls == NULL) {
		return NULL;
	}

	eye_memzero(ls, sizeof(eye_net_listening_t));

	sa = eye_palloc(loop->pool, socklen);
	if (sa == NULL) {
		return NULL;
	}

	eye_memcpy(sa, sockaddr, socklen);

	ls->sockaddr = sa;
	ls->socklen = socklen;

	len = eye_sock_ntop(sa, socklen, text, EYE_SOCKADDR_STRLEN, 1);
	ls->addr_text.len = len;

	switch (ls->sockaddr->sa_family) {
#if (EYE_HAVA_INET6)
	case AF_INET6:
		ls->addr_text_max_len = EYE_INET6_ADDRSTRLEN;
		break;
#endif
	case AF_INET:
		ls->addr_text_max_len = EYE_INET_ADDRSTRLEN;
		break;
	default:
		ls->addr_text_max_len = EYE_SOCKADDR_STRLEN;
		break;
	}

	ls->addr_text.data = eye_palloc(loop->pool, len);
	if (ls->addr_text.data == NULL) {
		return NULL;
	}

	eye_memcpy(ls->addr_text.data, text, len);

	ls->fd = (eye_socket_t) -1;
	ls->type = SOCK_STREAM;

	ls->backlog = EYE_LISTEN_BACKLOG;
	ls->rcvbuf = -1;
	ls->sndbuf = -1;
	ls->loop = loop;

	return ls;
}


eye_int_t eye_open_listening_sockets(eye_event_loop_t *loop)
{
	int						 reuseaddr;
	eye_err_t				 err;
	eye_uint_t				 i, tries, failed;
	eye_socket_t      		 s;
	eye_net_listening_t		*ls;

	reuseaddr = 1;
	failed = 0;

	for (tries = 5; tries; tries--) {
		failed = 0;
		ls = loop->listening.elts;

		for (i = 0; i< loop->listening.nelts; i++) {
			if (ls[i].fd != (eye_socket_t) -1) {
				continue;
			}

			s = eye_socket(ls[i].sockaddr->sa_family, ls[i].type, 0);

			if (s == (eye_socket_t) -1) {
				// log
				return EYE_ERROR;
			}

			if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const void *) &reuseaddr, sizeof(int)) == -1) {
				// log

				if (eye_close_socket(s) == -1) {
					// log;
				}

				return EYE_ERROR;
			}

			if (eye_nonblocking(s) == -1) {
				// log

				if (eye_close_socket(s) == -1) {
					// log
				}

				return EYE_ERROR;
			}

			if (bind(s, ls[i].sockaddr, ls[i].socklen) == -1) {
				err = eye_socket_errno;
				// log

				if (err != EYE_EADDRINUSE) {
					// log
				}

				if (eye_close_socket(s) == -1) {
					// log
				}

				if (err != EYE_EADDRINUSE) {
					return EYE_ERROR;
				}

				failed = 1;
				continue;
			}

			if (ls[i].type != SOCK_STREAM) {
				ls[i].fd = s;
				continue;
			}

			if (listen(s, ls[i].backlog) == -1) {
				err = eye_socket_errno;

				if (err != EYE_EADDRINUSE) {
					// log
				}

				if (eye_close_socket(s) == -1) {
					// log
				}

				if (err != EYE_EADDRINUSE) {
					return EYE_ERROR;
				}

				failed = 1;
				continue;
			}

			ls[i].listen = 1;
			ls[i].fd = s;
		}

		if (!failed) {
			break;
		}

		// log

		eye_msleep(500);
	}

    if (failed) {
    	// log
        return EYE_ERROR;
    }

	return EYE_OK;
}


void eye_configure_listening_sockets(eye_event_loop_t *loop)
{
	eye_uint_t				 i;
	eye_net_listening_t		*ls;

	ls = loop->listening.elts;
	for (i = 0; i < loop->listening.nelts; i++) {

		if (ls[i].rcvbuf != -1) {
			if (setsockopt(ls[i].fd, SOL_SOCKET, SO_RCVBUF, 
				(const void *) &ls[i].rcvbuf, sizeof(int))
				== -1)
			{
				// log
			}
		}

		if (ls[i].rcvbuf != -1) {
			if (setsockopt(ls[i].fd, SOL_SOCKET, SO_SNDBUF, 
				(const void *) &ls[i].sndbuf, sizeof(int))
				== -1)
			{
				// log
			}
		}

		if (ls[i].listen) {

			if (listen(ls[i].fd, ls[i].backlog) == -1) {
				// log
			}
		}
	}
}