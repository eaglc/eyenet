

#include "eye_core.h"


eye_int_t	eye_ncpu;

eye_int_t eye_os_init()
{
	if (eye_ncpu == 0) {
		eye_ncpu = sysconf(_SC_NPROCESSORS_ONLN);
	}

	if (eye_ncpu < 1) {
		eye_ncpu = 1;
	}


	return EYE_OK;
}