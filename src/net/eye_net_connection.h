

#ifndef __EYE_NET_CONNECTION_H__
#define __EYE_NET_CONNECTION_H__


#include "eye_core.h"

struct eye_net_connection_s {
	void               	*data;
	eye_event_t 		*read;
	eye_event_t 		*write;

	eye_socket_t 		fd;

    eye_recv_pt         recv;
    eye_send_pt         send;

    eye_net_listening_t    *listening;

    eye_pool_t         *pool;

    int                 type;

    struct sockaddr    *sockaddr;
    socklen_t           socklen;
    eye_str_t           addr_text;


    struct sockaddr    *local_sockaddr;
    socklen_t           local_socklen;

    eye_buf_t          *buffer;

    eye_atomic_uint_t   number;

    unsigned            timedout:1;
    unsigned            error:1;
    unsigned            destroyed:1;

    unsigned            close:1;
};


eye_int_t eye_net_connection_error(eye_net_connection_t *c, eye_err_t err, char *text);

eye_net_connection_t * eye_get_connection(eye_event_loop_t *loop, eye_socket_t s);

void eye_free_connection(eye_net_connection_t *c);
void eye_close_connection(eye_net_connection_t *c);

#endif /*__EYE_NET_CONNECTION_H__*/