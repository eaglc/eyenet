

#ifndef __EYE_ARRAY_H__
#define __EYE_ARRAY_H__


#include "eye_core.h"


typedef struct {
	void		*elts;		
	eye_uint_t	 nelts;		// 数组元素个数
	size_t		 size;		// 数组元素大小
	eye_uint_t	 nalloc;	// 数组容量
	eye_pool_t	*pool;
} eye_array_t;

eye_array_t *eye_array_create(eye_pool_t *p, eye_uint_t n, size_t size);
void eye_array_destroy(eye_array_t *a);
void *eye_array_push(eye_array_t *a);
void *eye_array_push_n(eye_array_t *a, eye_uint_t n);


static eye_inline eye_int_t 
eye_array_init(eye_array_t *array, eye_pool_t *pool, eye_uint_t n, size_t size)
{
	array->nelts = 0;
	array->nalloc = n;
	array->size = size;
	array->pool = pool;

	array->elts = eye_palloc(pool, n * size);
	if (array->elts == NULL) {
		return EYE_ERROR;
	}

	return EYE_OK;
}


#endif /*__EYE_ARRAY_H__*/