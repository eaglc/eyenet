

#ifndef __EYE_CORE_H__
#define __EYE_CORE_H__


#include "eye_core.h"


typedef int               eye_err_t;

#define EYE_EPERM         EPERM
#define EYE_ENOENT        ENOENT
#define EYE_ENOPATH       ENOENT
#define EYE_ESRCH         ESRCH
#define EYE_EINTR         EINTR
#define EYE_ECHILD        ECHILD
#define EYE_ENOMEM        ENOMEM
#define EYE_EACCES        EACCES
#define EYE_EBUSY         EBUSY
#define EYE_EEXIST        EEXIST
#define EYE_EEXIST_FILE   EEXIST
#define EYE_EXDEV         EXDEV
#define EYE_ENOTDIR       ENOTDIR
#define EYE_EISDIR        EISDIR
#define EYE_EINVAL        EINVAL
#define EYE_ENFILE        ENFILE
#define EYE_EMFILE        EMFILE
#define EYE_ENOSPC        ENOSPC
#define EYE_EPIPE         EPIPE
#define EYE_EINPROGRESS   EINPROGRESS
#define EYE_ENOPROTOOPT   ENOPROTOOPT
#define EYE_EOPNOTSUPP    EOPNOTSUPP
#define EYE_EADDRINUSE    EADDRINUSE
#define EYE_ECONNABORTED  ECONNABORTED
#define EYE_ECONNRESET    ECONNRESET
#define EYE_ENOTCONN      ENOTCONN
#define EYE_ETIMEDOUT     ETIMEDOUT
#define EYE_ECONNREFUSED  ECONNREFUSED
#define EYE_ENAMETOOLONG  ENAMETOOLONG
#define EYE_ENETDOWN      ENETDOWN
#define EYE_ENETUNREACH   ENETUNREACH
#define EYE_EHOSTDOWN     EHOSTDOWN
#define EYE_EHOSTUNREACH  EHOSTUNREACH
#define EYE_ENOSYS        ENOSYS
#define EYE_ECANCELED     ECANCELED
#define EYE_EILSEQ        EILSEQ
#define EYE_ENOMOREFILES  0
#define EYE_ELOOP         ELOOP
#define EYE_EBADF         EBADF

#define EYE_EAGAIN        EAGAIN


#define eye_errno                  errno
#define eye_socket_errno           errno
#define eye_set_errno(err)         errno = err
#define eye_set_socket_errno(err)  errno = err


// u_char *eye_strerror(eye_err_t err, u_char *errstr, size_t size);
// eye_int_t eye_strerror_init(void);