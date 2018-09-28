#ifndef __EYE_TIME_H__
#define __EYE_TIME_H__

typedef eye_uint_t  eye_msec_t;
typedef eye_int_t	eye_msec_int_t;

#define eye_gettimeofday(tp)  (void) gettimeofday(tp, NULL);
#define eye_msleep(ms)        (void) usleep(ms * 1000)
#define eye_sleep(s)          (void) sleep(s)

#endif