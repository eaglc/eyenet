#ifndef __EYE_BUF_H__
#define __EYE_BUF_H__


#include "eye_core.h"


struct eye_buf_s {
	char *start;
	char *end;
	char *pos;
	char *last;
};


#endif