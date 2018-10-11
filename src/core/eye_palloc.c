
#include "eye_core.h"

static eye_inline void *eye_palloc_small(eye_pool_t *pool, size_t size, uint32_t align);
static void *eye_palloc_block(eye_pool_t *pool, size_t size);
static void *eye_palloc_large(eye_pool_t *pool, size_t size);


eye_pool_t *
	eye_create_pool(size_t size)
{
	eye_pool_t *p;

	p = (eye_pool_t *)eye_memalign(EYE_POOL_ALIGNMENT, size);
	if (p == NULL) {
		return NULL;
	}

	p->d.last = (u_char *)p + sizeof(eye_pool_t);
	p->d.end = (u_char *)p + size;
	p->d.next = NULL;
	p->d.failed = 0;

	size = size - sizeof(eye_pool_t);
	p->max = (size < EYE_MAX_ALLOC_FROM_POOL) ? size : EYE_MAX_ALLOC_FROM_POOL;

	p->current = p;
	p->large = NULL;
	p->cleanup = NULL;

	return p;
}

void 
	eye_destroy_pool(eye_pool_t *pool)
{
	eye_pool_t			*p, *n;
	eye_pool_large_t	*l;
	eye_pool_cleanup_t	*c;

	// 先执行cleanup
	for (c = pool->cleanup; c; c = c->next) {
		if (c->handler) {
			c->handler(c->data);
		}
	}

	// 释放大块内存
	for (l = pool->large; l; l = l->next) {
		if (l->alloc) {
			eye_free(l->alloc);
		}
	}

	// 释放内存池内存
	for (p = pool, n = pool->d.next; ; p = n, n = n->d.next) {
		eye_free(p);

		if (n == NULL) {
			break;
		}
	}

}

void 
	eye_reset_pool(eye_pool_t *pool)
{
	// 释放大块内存并标记内存池可用
	eye_pool_t			*p;
	eye_pool_large_t	*l;

	for (l = pool->large; l; l = l->next) {
		if (l->alloc) {
			eye_free(l->alloc);
		}
	}

	for (p = pool; p; p = p->d.next) {
		p->d.last = (u_char *)p + sizeof(eye_pool_t);
		p->d.failed = 0;
	}

	pool->current = pool;
	pool->large = NULL;
}

void *
	eye_palloc(eye_pool_t *pool, size_t size)
{
	if (size <= pool->max) {
		return eye_palloc_small(pool, size, 1);
	}

	return eye_palloc_large(pool, size);
}

void *
	eye_pnalloc(eye_pool_t *pool, size_t size)
{
	if (size <= pool->max) {
		return eye_palloc_small(pool, size, 0);
	}

	return eye_palloc_large(pool, size);
}


static eye_inline void *
	eye_palloc_small(eye_pool_t *pool, size_t size, uint32_t align)
{
	u_char		*m;
	eye_pool_t	*p;

	p = pool->current;

	do 
	{
		m = p->d.last;

		if (align) {
			m = eye_align_ptr(m, EYE_ALIGNMENT);
		}

		if ((size_t) (p->d.end - m) >= size) {
			p->d.last = m + size;

			return m;
		}

		p = p->d.next;
	} while (p);

	return eye_palloc_block(pool, size);
}

static void *
	eye_palloc_block(eye_pool_t *pool, size_t size)
{
	u_char		*m;
	size_t		 psize;
	eye_pool_t	*p, *np;

	psize = (size_t) (pool->d.last - (u_char *)pool);

	m = (u_char *)eye_memalign(EYE_POOL_ALIGNMENT, psize);

	if (m == NULL) {
		return NULL;
	}

	np = (eye_pool_t *) m;
	np->d.end = m + psize;
	np->d.next = NULL;
	np->d.failed = 0;

	m += sizeof(eye_pool_data_t);
	m = eye_align_ptr(m, EYE_ALIGNMENT);
	np->d.last = m + size;

	for (p = pool->current; p->d.next; p = p->d.next) {
		if (p->d.failed++ > 4) {
			pool->current = p->d.next;
		}
	}

	p->d.next = np;

	return m;
}

static void *
	eye_palloc_large(eye_pool_t *pool, size_t size)
{
	void				*p;
	uint32_t			 n;
	eye_pool_large_t	*large;

	p = eye_alloc(size);
	if (p == NULL) {
		return p;
	}

	n = 0;

	for (large = pool->large; large; large = large->next) {
		if (large->alloc == NULL) {
			large->alloc = p;
			return p;
		}

		if (n++ > 3) {
			break;
		}
	}

	large = (eye_pool_large_t *)eye_palloc_small(pool, sizeof(eye_pool_large_t), 1);
	if (large == NULL) {
		eye_free(p);
		return NULL;
	}

	large->alloc = p;
	large->next = pool->large;
	pool->large = large;

	return p;
}



void *
	eye_pmemalign(eye_pool_t *pool, size_t size, size_t aligment)
{
	void				*p;
	eye_pool_large_t	*large;

	p = eye_memalign(aligment, size);
	if (p == NULL) {
		return NULL;
	}

	large = (eye_pool_large_t *)eye_palloc_small(pool, sizeof(eye_pool_large_t), 1);
	if (large == NULL) {
		eye_free(p);
		return NULL;
	}

	large->alloc = p;
	large->next = pool->large;
	pool->large = large;

	return p;
}

int eye_pfree(eye_pool_t *pool, void *p)
{
	eye_pool_large_t		*l;

	for (l = pool->large; l; l = l->next) {
		if (p == l->alloc) {
			eye_free(l->alloc);
			l->alloc = NULL;

			return 0;
		}
	}

	return -6;	// tttttttttttttttttttttt
}

void *
	eye_pcalloc(eye_pool_t *pool, size_t size)
{
	void *p;

	p = eye_palloc(pool, size);

	if (p) {
		eye_memzero(p, size);
	}

	return p;
}

eye_pool_cleanup_t *
	eye_pool_cleanup_add(eye_pool_t *p, size_t size)
{
	eye_pool_cleanup_t *c;

	c = (eye_pool_cleanup_t *)eye_palloc(p, sizeof(eye_pool_cleanup_t));
	if (c == NULL) {
		return NULL;
	}

	if (size) {
		c->data = eye_palloc(p, size);
		if (c->data == NULL) {
			return NULL;
		}
	} else {
		c->data = NULL;
	}

	c->handler = NULL;
	c->next = p->cleanup;

	p->cleanup = c;

	return c;
}

void 
	eye_pool_run_cleanup_file(eye_pool_t *p, int fd)
{
	eye_pool_cleanup_t			*c;
	eye_pool_cleanup_file_t		*cf;

	for (c = p->cleanup; c; c = c->next) {
		if (c->handler == eye_pool_cleanup_file) {
			cf = (eye_pool_cleanup_file_t *)c->data;

			if (cf->fd == fd) {
				c->handler(cf);
				c->handler = NULL;
				return;
			}
		}
	}
}

void 
	eye_pool_cleanup_file(void *data)
{
	//eye_pool_cleanup_file_t		*c = (eye_pool_cleanup_file_t *)data;

	// tttttttttttttt
	//if (close(c->fd) == -1) {
	//	// log
	//}
}

void 
	eye_pool_delete_file(void *data)
{

}