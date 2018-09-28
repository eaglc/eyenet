#ifndef __EYE_ATOMIC_H__
#define __EYE_ATOMIC_H__

#include "eye_core.h"

#include <stdint.h>
#include <inttypes.h>

#define eye_cpu_pause()

#define eye_atomic_fetch_add(value, add)					\
	__sync_fetch_and_add(value, add)

#define eye_atomic_add_fetch(value, add)					\
	__sync_add_and_fetch(value, add)

#define eye_atomic_cmp_set(lock, old, set)					\
	__sync_bool_compare_and_swap(lock, old, set)

typedef int32_t 						eye_atomic_int32_t;
typedef uint32_t						eye_aomic_uint32_t;
typedef int64_t							eye_atomic_int64_t;
typedef uint64_t						eye_atomic_uint64_t;
typedef long							eye_atomic_int_t;
typedef unsigned long					eye_atomic_uint_t;
typedef volatile eye_atomic_uint_t 		eye_atomic_t;


#if (EYE_PTR_SIZE == 8)
#define EYE_ATOMIC_T_LEN            (sizeof("-9223372036854775808") - 1)
#else
#define EYE_ATOMIC_T_LEN            (sizeof("-2147483648") - 1)
#endif



#endif
