
#ifndef __EYE_PALLOC_H__
#define __EYE_PALLOC_H__


#include "eye_core.h"


typedef struct eye_pool_s		eye_pool_t;

// tttttttttttt
#define EYE_MAX_ALLOC_FROM_POOL		4095

#define EYE_DEFAULT_POOL_SIZE		(16 * 1024)

#define EYE_POOL_ALIGNMENT			16

#define EYE_MIN_POOL_SIZE                                                     \
	eye_align((sizeof(eye_pool_t) + 2 * sizeof(eye_pool_large_t)),            \
	EYE_POOL_ALIGNMENT)

typedef void (*eye_pool_cleanup_pt)(void *data);

typedef struct eye_pool_cleanup_s  eye_pool_cleanup_t;

struct eye_pool_cleanup_s {
	eye_pool_cleanup_pt   handler;
	void				 *data;
	eye_pool_cleanup_t   *next;
};


typedef struct eye_pool_large_s  eye_pool_large_t;

struct eye_pool_large_s {
	eye_pool_large_t    *next;
	void				*alloc;
};


typedef struct {
	u_char			*last;
	u_char			*end;
	eye_pool_t		*next;
	uint32_t		failed;				// tttttttttttt
}eye_pool_data_t;


struct eye_pool_s {
	eye_pool_data_t			 d;
	size_t					 max;
	eye_pool_t				*current;
	eye_pool_large_t		*large;
	eye_pool_cleanup_t		*cleanup;
};


typedef struct {
	int			 fd;			// ttttttttttt
	u_char		*name;
}eye_pool_cleanup_file_t;


eye_pool_t *eye_create_pool(size_t size);
void eye_destroy_pool(eye_pool_t *pool);
void eye_reset_pool(eye_pool_t *pool);


void *eye_palloc(eye_pool_t *pool, size_t size);
void *eye_pnalloc(eye_pool_t *pool, size_t size);
void *eye_pcalloc(eye_pool_t *pool, size_t size);
void *eye_pmemalign(eye_pool_t *pool, size_t size, size_t aligment);
int  eye_pfree(eye_pool_t *pool, void *p);


eye_pool_cleanup_t *eye_pool_cleanup_add(eye_pool_t *pool, size_t size);
void eye_pool_run_cleanup_file(eye_pool_t *p, int fd);
void eye_pool_cleanup_file(void *data);
void eye_pool_delete_file(void *data);


#endif