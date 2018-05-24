
#ifndef __EYE_PROCESS_H__
#define __EYE_PROCESS_H__

#if 1
#define eye_sched_yield()  sched_yield()
#else
#define eye_sched_yield()	usleep(1)
#endif


#endif