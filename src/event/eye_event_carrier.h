#ifndef __EYE_EVENT_CARRIER_H__
#define __EYE_EVENT_CARRIER_H__

#include "eye_core.h"

typedef eye_event_carrier_s		eye_event_carrier_t;

struct eye_event_carrier_s {
	void 			*data;
	eye_event_t 	*read;
	eye_event_t 	*write;

	eye_socket_t 	fd;
};


#endif