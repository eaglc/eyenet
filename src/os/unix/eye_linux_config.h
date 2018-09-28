

#ifndef __EYE_LINUX_CONFIG_H__
#define __EYE_LINUX_CONFIG_H__


#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdarg.h>
#include <stddef.h>             /* offsetof() */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <glob.h>
#include <sys/vfs.h>            /* statfs() */

#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sched.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>        /* TCP_NODELAY, TCP_CORK */
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>

#include <time.h>               /* tzset() */
#include <malloc.h>             /* memalign() */
#include <limits.h>             /* IOV_MAX */
#include <sys/ioctl.h>
#include <crypt.h>
#include <sys/utsname.h>        /* uname() */

#include <dlfcn.h>

#include <sys/epoll.h>

#include <pthread.h>

typedef intptr_t			eye_int_t;
typedef uintptr_t			eye_uint_t;

typedef intptr_t			eye_intptr_t;
typedef uintptr_t			eye_uintptr_t;

typedef int32_t				eye_int32_t;
typedef uint32_t			eye_uint32_t;

typedef int64_t				eye_int64_t;
typedef uint64_t			eye_uint64_t;


#define EYE_LISTEN_BACKLOG	511

#endif /*__EYE_LINUX_CONFIG_H__*/