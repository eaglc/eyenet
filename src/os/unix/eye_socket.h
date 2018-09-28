

#ifndef __EYE_SOCKET_H__
#define __EYE_SOCKET_H__


typedef int  eye_socket_t;

#define eye_socket          socket

#define eye_nonblocking(s)	fcntl(s, F_SETFL, fcntl(s, F_GETFL) | O_NONBLOCK)
#define eye_blocking(s)		fcntl(s, F_SETFL, fcntl(s, F_GETFL) & ~O_NONBLOCK)

#define eye_shutdown_socket	shutdown
#define eye_close_socket	close


#endif