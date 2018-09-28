
#ifndef __EYE_CORE_H__
#define __EYE_CORE_H__

#include "eye_config.h"


typedef struct eye_event_s				eye_event_t;
typedef struct eye_event_loop_s			eye_event_loop_t;
typedef struct eye_event_loop_thread_s  eye_event_loop_thread_t;

typedef struct eye_net_connection_s		eye_net_connection_t;
typedef struct eye_net_listening_s		eye_net_listening_t;

#define  EYE_OK          0
#define  EYE_ERROR      -1
#define  EYE_AGAIN      -2
#define  EYE_BUSY       -3
#define  EYE_DONE       -4
#define  EYE_DECLINED   -5
#define  EYE_ABORT      -6

#include "eye_errno.h"
#include "eye_atomic.h"
#include "eye_spinlock.h"
#include "eye_palloc.h"
#include "eye_time.h"
#include "eye_times.h"
#include "eye_str.h"
#include "eye_io.h"
#include "eye_array.h"
#include "eye_buf.h"
#include "eye_queue.h"
#include "eye_rbtree.h"
#include "eye_rbtree_imp.h"
#include "eye_socket.h"
#include "eye_os.h"
#include "eye_process.h"
#include "eye_alloc.h"
#include "eye_palloc.h"
#include "eye_pthread.h"
#include "eye_event_timer.h"
#include "eye_event_loop.h"
#include "eye_event_loop_thread.h"
#include "eye_inet.h"

#include "eye_net_connection.h"
#include "eye_net_listening.h"
#include "eye_net_accept.h"


#define eye_abs(val)		(((val) >= 0) ? (val) : - (val))
#define eye_max(val1, val2) (((val1) > (val2)) ? (val1) : (val2))
#define eye_min(val1, val2) (((val1) > (val2)) ? (val2) : (val1))


#define LF     (u_char) '\n'
#define CR     (u_char) '\r'
#define CRLF   "\r\n"



#endif /*__EYE_CORE_H__*/