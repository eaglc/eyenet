

#include "eye_core.h"
#include "eye_event.h"

ssize_t eye_unix_send(eye_net_connection_t *c, u_char *buf, size_t size)
{
    ssize_t       n;
    eye_err_t     err;
    eye_event_t  *wev;

    wev = c->write;

    for ( ;; ) {
        n = send(c->fd, buf, size, 0);

        if (n > 0) {
            if (n < (ssize_t) size) {
                wev->ready = 0;
            }
            
            return n;
        }

        err = eye_socket_errno;

        if (n == 0) {
            wev->ready = 0;
            return n;
        }

        if (err == EYE_EAGAIN || err == EYE_EINTR) {
            wev->ready = 0;


            if (err == EYE_EAGAIN) {
                return EYE_AGAIN;
            }

        } else {
            wev->error = 1;
            //(void) eye_connection_error(c, err, "send() failed");
            return EYE_ERROR;
        }
    }
}