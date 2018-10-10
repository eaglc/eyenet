
#ifndef __EYE_OS_H__
#define __EYE_OS_H__


#include "eye_core.h"


typedef ssize_t (*eye_recv_pt)(eye_net_connection_t *c, u_char *buf, size_t size);
//typedef ssize_t (*eye_recv_chain_pt)(eye_net_connection_t *c, eye_chain_t *in,
//    off_t limit);
typedef ssize_t (*eye_send_pt)(eye_net_connection_t *c, u_char *buf, size_t size);
//typedef eye_chain_t *(*eye_send_chain_pt)(eye_net_connection_t *c, eye_chain_t *in,
//    off_t limit);

typedef struct {
    eye_recv_pt        recv;
//    eye_recv_chain_pt  recv_chain;
    eye_recv_pt        udp_recv;
    eye_send_pt        send;
    eye_send_pt        udp_send;
    // eye_send_chain_pt  udp_send_chain;
    // eye_send_chain_pt  send_chain;
    eye_uint_t         flags;
} eye_os_io_t;


eye_int_t eye_os_init();


extern eye_int_t	eye_ncpu;
extern eye_os_io_t  eye_os_io;


ssize_t eye_unix_recv(eye_net_connection_t *c, u_char *buf, size_t size);
ssize_t eye_udp_unix_recv(eye_net_connection_t *c, u_char *buf, size_t size);
ssize_t eye_unix_send(eye_net_connection_t *c, u_char *buf, size_t size);
ssize_t eye_udp_unix_send(eye_net_connection_t *c, u_char *buf, size_t size);


#endif