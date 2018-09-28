

#include "eye_core.h"


void eye_spinlock(eye_atomic_t *lock, eye_atomic_int_t value, eye_uint_t spin)
{
    eye_uint_t  i, n;

    for ( ;; ) {

        if (*lock == 0 && eye_atomic_cmp_set(lock, 0, value)) {
            return;
        }

        if (eye_ncpu > 1) {

            for (n = 1; n < spin; n <<= 1) {

                for (i = 0; i < n; i++) {
                    eye_cpu_pause();
                }

                if (*lock == 0 && eye_atomic_cmp_set(lock, 0, value)) {
                    return;
                }
            }
        }
        
        eye_sched_yield();
    }
}