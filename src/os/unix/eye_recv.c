

#include "eye_core.h"
#include "eye_event.h"


ssize_t eye_unix_recv(eye_net_connection_t *c, u_char *buf, size_t size)
{
	ssize_t       n;
    eye_err_t     err;
    eye_event_t  *rev;

    rev = c->read;

    if (!rev->available && !rev->pending_eof) {
        rev->ready = 0;
        return EYE_AGAIN;
    }

    do {
        n = recv(c->fd, buf, size, 0);

        if (n == 0) {
            rev->ready = 0;
            rev->eof = 1;

            return 0;
        }

        if (n > 0) {

            if ((size_t) n < size) {
                if (!rev->pending_eof) {
                    rev->ready = 0;
                }

                rev->available = 0;
            }

            return n;
        }

        err = eye_socket_errno;

        if (err == EYE_EAGAIN || err == EYE_EINTR) {
            n = EYE_AGAIN;
        } else {
            n = eye_net_connection_error(c, err, "recv() failed");
            break;
        }

    } while (err == EYE_EINTR);

    rev->ready = 0;

    if (n == EYE_ERROR) {
        rev->error = 1;
    }

    return n;
}