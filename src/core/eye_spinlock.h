#ifndef __EYE_SPINLOCK_H__
#define __EYE_SPINLOCK_H__

#include "eye_core.h"

void eye_spinlock(eye_atomic_t *lock, eye_atomic_int_t value, eye_uint_t spin);

#define eye_lock(lock) 		eye_spinlock(lock, 1, 2048)
#define eye_trylock(lock) 	(*(lock) == 0 && eye_atomic_cmp_set(lock, 0, 1))
#define eye_unlock(lock)   	 *(lock) = 0

#endif