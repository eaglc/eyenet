#ifndef __EYE_BUF_H__
#define __EYE_BUF_H__


#include "eye_core.h"


typedef struct eye_buf_s 	eye_buf_t;

struct eye_buf_s {
	u_char *start;
	u_char *end;
	u_char *pos;
	u_char *last;
};

//
// writeable = end - last
// readable = pos - start

#define eye_alloc_buf(pool)   eye_palloc(pool, sizeof(eye_buf_t))
#define eye_calloc_buf(pool)  eye_pcalloc(pool, sizeof(eye_buf_t))

eye_buf_t *eye_create_buf(eye_pool_t *pool, size_t size);



#endif