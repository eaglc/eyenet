

#ifndef __EYE_TIMES_H__
#define __EYE_TIMES_H__


#include "eye_core.h"


static eye_inline eye_msec_t eye_monotonic_time(time_t sec, eye_uint_t msec)
{
	struct timeval	tv;
	// clock_get_time();

	eye_gettimeofday(&tv);
	sec = tv.tv_sec;
	msec = tv.tv_usec / 1000;

	return (eye_msec_t) sec * 1000 + msec;
}


#endif