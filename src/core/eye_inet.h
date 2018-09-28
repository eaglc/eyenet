

#ifndef __EYE_INET_H__
#define __EYE_INET_H__


// todo: 这一块好好看下 ipv4 ipv6 以及对应的数据结构

#include "eye_core.h"

#define EYE_INET_ADDRSTRLEN		(sizeof("255.255.255.255") - 1)
#define EYE_INET6_ADDRSTRLEN												\
	(sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255") - 1)


#if (EYE_HAVE_INET6)
#define EYE_SOCKADDR_STRLEN		(EYE_INET6_ADDRSTRLEN + sizeof("[]:65535") - 1)
#else
#define EYE_SOCKADDR_STRLEN		(EYE_INET_ADDRSTRLEN + sizeof(":65535") - 1)
#endif


typedef union {
	struct sockaddr			sockaddr;
	struct sockaddr_in		sockaddr_in;
#if (EYE_HAVE_INET6)
	struct sockaddr_in6		sockaddr_in6;
#endif
} eye_sockaddr_t;


typedef struct {
	struct sockaddr			*sockaddr;
	socklen_t				 socklen;
	eye_str_t				 name;
} eye_addr_t;


in_addr_t eye_inet_addr(u_char *text, size_t len);
#if (EYE_HAVE_INET6)
eye_int_t eye_inet6_addr(u_char *p, size_t len, u_char *addr);
size_t eye_inet6_ntop(u_char *p, u_char *text, size_t len);
#endif

size_t eye_sock_ntop(struct sockaddr *sa, socklen_t socklen, u_char *text, size_t len, eye_uint_t port);
size_t eye_inet_ntop(int family, void *addr, u_char *text, size_t len);

in_port_t eye_inet_get_port(struct sockaddr *sa);
void eye_inet_set_port(struct sockaddr *sa, in_port_t port);

eye_int_t eye_parse_addr(eye_pool_t *pool, eye_addr_t *addr, u_char *text, size_t len);
eye_int_t eye_parse_addr_port(eye_pool_t *pool, eye_addr_t *addr, u_char *text, size_t len);


#endif