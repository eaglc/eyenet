

#ifndef __EYE_CONFIG_H__
#define __EYE_CONFIG_H__


#if (EYE_LINUX)
#include "eye_linux_config.h"
#endif /*EYE_LINUX*/

#define EYE_ALIGNMENT   sizeof(unsigned long)    /* platform word */


#define  eye_align(d, a) (((d) + (a - 1)) & ~(a - 1))
#define  eye_align_ptr(p, a)												\
	(u_char *)(((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))


#ifndef eye_inline
#define eye_inline      inline
#endif


#ifndef INADDR_NONE  /* Solaris */
#define INADDR_NONE  ((unsigned int) -1)
#endif


#define EYE_INT32_LEN   (sizeof("-2147483648") - 1)
#define EYE_INT64_LEN   (sizeof("-9223372036854775808") - 1)

#if (EYE_PTR_SIZE == 4)
#define EYE_INT_T_LEN   		EYE_INT32_LEN
#define EYE_MAX_INT_T_VALUE  	2147483647
#define EYE_SIZE_T_LEN			(sizeof("-2147483648") - 1)
#define EYE_MAX_SIZE_T_VALUE	2147483647
#define EYE_TIME_T_LEN			(sizeof("-2147483648") - 1)
#define EYE_TIME_T_SIZE			4
#define EYE_MAX_TIME_T_VALUE	2147483647

#else
#define EYE_INT_T_LEN   		EYE_INT64_LEN
#define EYE_MAX_INT_T_VALUE  	9223372036854775807
#define EYE_SIZE_T_LEN			(sizeof("-9223372036854775808") - 1)
#define EYE_MAX_SIZE_T_VALUE	9223372036854775807
#define EYE_TIME_T_LEN			(sizeof("-9223372036854775808") - 1)
#define EYE_TIME_T_SIZE			8
#define EYE_MAX_TIME_T_VALUE	9223372036854775807
#endif


#define EYE_MAX_UINT32_VALUE  (uint32_t) 0xffffffff
#define EYE_MAX_INT32_VALUE   (uint32_t) 0x7fffffff


#define EYE_OFF_T_LEN           (sizeof("-9223372036854775807") - 1)
#define EYE_MAX_OFF_T_VALUE     9223372036854775807


#endif /*__EYE_CONFIG_H__*/

