

#ifndef __EYE_NET_LISTENING_H__
#define __EYE_NET_LISTENING_H__


#include "eye_core.h"


typedef void (*eye_connection_handler_pt)(eye_net_connection_t *c);

struct eye_net_listening_s {
   	eye_socket_t        fd;

    struct sockaddr    *sockaddr;
    socklen_t           socklen;    /* size of sockaddr */
    size_t              addr_text_max_len;
    eye_str_t           addr_text;

    int                 type;

    int                 backlog;
    int                 rcvbuf;
    int                 sndbuf;

    /* handler of accepted connection */
    eye_connection_handler_pt   handler;

    size_t              pool_size;

    eye_net_connection_t   *connection;

    eye_event_loop_t        *loop;

    eye_uint_t          worker;

    unsigned            open:1;

    unsigned            listen:1;
    unsigned            nonblocking:1;
    unsigned			reuseport:1;
    unsigned            addr_ntop:1;
};


eye_net_listening_t *eye_create_listening(eye_event_loop_t *loop, struct sockaddr *sockaddr, socklen_t socklen);

eye_int_t eye_open_listening_sockets(eye_event_loop_t *loop);

void eye_configure_listening_sockets(eye_event_loop_t *loop);

#endif /*__EYE_NET_LISTENING_H__*/