

#include "eye_core.h"

int main()
{
	eye_event_loop_t 		*loop;
	eye_net_listening_t 	*ls;
	eye_net_connection_t 	*c;
	char 					*ip;
	eye_int_t 				 port;

	loop = eye_event_loop_init(1);

	if (loop == NULL) {
		return -1;
	}

	*ip = "127.0.0.1";
	port = 17170;

	


	return 0;
}