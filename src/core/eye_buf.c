

#include "eye_core.h"

eye_buf_t *eye_create_buf(eye_pool_t *pool, size_t size)
{
	eye_buf_t *b;

	b = eye_calloc_buf(pool);
	if (b == NULL) {
		return NULL;
	}

	b->start = eye_palloc(pool, size);
	if (b->start == NULL) {
		return NULL;
	}

	b->pos = b->start;
	b->last = b->start;
	b->end = b->last+size;

	return b;
}