

#include "eye_core.h"


void *eye_alloc(size_t size)
{
	void *p;

	p = malloc(size);
	if (p == NULL) {
		// log
	}

	return p;
}

void *eye_calloc(size_t size)
{
	void *p;

	p = eye_alloc(size);

	if (p) {
		eye_memzero(p, size);
	}

	return p;
}

void *
eye_memalign(size_t alignment, size_t size)
{
    void  *p;
    int    err;

    err = posix_memalign(&p, alignment, size);

    if (err) {
        //eye_log_error(EYE_LOG_EMERG, log, err,
        //              "posix_memalign(%uz, %uz) failed", alignment, size);
        p = NULL;
    }

    //eye_log_debug3(EYE_LOG_DEBUG_ALLOC, log, 0,
    //               "posix_memalign: %p:%uz @%uz", p, size, alignment);

    return p;
}