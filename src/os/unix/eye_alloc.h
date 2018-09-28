
#ifndef __EYE_ALLOC_H__
#define __EYE_ALLOC_H__


#include "eye_core.h"


void *eye_alloc(size_t size);
void *eye_calloc(size_t size);

void *eye_memalign(size_t alignment, size_t size/*, eye_log_t *log*/);

#define eye_free		free

#endif