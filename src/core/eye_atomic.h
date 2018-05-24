#ifndef __EYE_ATOMIC_H__


#include "eye_core.h"


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


#define __EYE_ATOMIC_H__