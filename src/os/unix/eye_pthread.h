

#ifndef __EYE_PTHREAD_H__
#define __EYE_PTHREAD_H__


#include "eye_core.h"

typedef pthread_t	eye_pthread_t;
typedef pthread_attr_t eye_pthread_attr_t;

typedef void *(*eye_thread_handler_pt) (void *);

#define eye_pthread_create(thread, attr, routine, arg) pthread_create(thread, attr, routine, arg) 
#define eye_pthread_join(thread, retval) pthread_join(thread, retval)
#define eye_pthread_exit(retval) pthread_exit(retval)


#endif