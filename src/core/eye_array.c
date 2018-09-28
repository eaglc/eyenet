

#include "eye_core.h"


eye_array_t *eye_array_create(eye_pool_t *p, eye_uint_t n, size_t size)
{
	eye_array_t *a;
	a = eye_palloc(p, sizeof(eye_array_t));
	if (a == NULL) {
		return NULL;
	}

	if (eye_array_init(a, p, n, size) != EYE_OK) {
		return NULL;
	}

	return a;
}


void eye_array_destroy(eye_array_t *a)
{
	// 并不真正free内存
	eye_pool_t *p;

	p = a->pool;

	if ((u_char *) a->elts + a->size * a->nalloc == p->d.last) {
		p->d.last -= a->size * a->nalloc;
	}

	if ((u_char *) a + sizeof(eye_array_t) == p->d.last) {
		p->d.last = (u_char *)a;
	}
}


void *eye_array_push(eye_array_t *a)
{
	void		*elt, *new;
	size_t		 size;
	eye_pool_t	*p;

	if (a->nelts == a->nalloc) {

		size = a->size * a->nalloc;

		p = a->pool;

		if ((u_char *) a->elts + size == p->d.last
			&& p->d.last + a->size <= p->d.end)
		{

			p->d.last += a->size;
			a->nalloc++;

		} else {
			// 这里并未释放内存
			new = eye_palloc(p, 2 * size);

			if (new == NULL) {
				return NULL;
			}

			eye_memcpy(new, a->elts, size);
			a->elts = new;
			a->nalloc *= 2;
		}
	}

	elt = (u_char *) a->elts + a->size * a->nelts;
	a->nelts++;

	return elt;
}


void *eye_array_push_n(eye_array_t *a, eye_uint_t n)
{
	void		*elt, *new;
	size_t		 size;
	eye_uint_t	 nalloc;
	eye_pool_t	*p;

	size = n * a->size;

	if (a->nelts + n > a->nalloc) {

		p = a->pool;

		if ((u_char *) a->elts + a->size * a->nalloc == p->d.last 
			&& p->d.last + size <= p->d.end)
		{
			p->d.last += size;
			a->nalloc += n;

		} else {
			nalloc = 2 * ((n >= a->nalloc) ? n : a->nalloc);

			new = eye_palloc(p, nalloc * a->size);

			if (new == NULL) {
				return NULL;
			}

			eye_memcpy(new, a->elts, a->nelts * a->size);
			a->elts = new;
			a->nalloc = nalloc;
		}
	}

	elt = (u_char *) a->elts + a->size * a->nelts;
	a->nelts += n;

	return elt;
}