
#ifndef __EYE_PROCESS_H__
#define __EYE_PROCESS_H__

#include "eye_core.h"

typedef pid_t       eye_pid_t;

#if 1
#define eye_sched_yield()  sched_yield()
#else
#define eye_sched_yield()	usleep(1)
#endif


#endif