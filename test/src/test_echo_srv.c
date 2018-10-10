

#include "eye_core.h"
#include <stdio.h>


u_char *ip = (u_char *)"127.0.0.1:17001";

void on_connected(eye_net_connection_t *c);
void on_data(eye_event_t *ev);
void on_write(eye_event_t *ev);
void on_closed(eye_net_connection_t *c);

int main()
{
	if (eye_os_init() != EYE_OK) {
		perror("eye_os_init failure\n");
		return -1;
	}

	eye_event_loop_t 		*loop;
	eye_net_listening_t 	*ls;

	loop = eye_event_loop_init(1);

	if (loop == NULL) {
		perror("eye_event_loop_init failure\n");
		return -2;
	}

	eye_addr_t addr;

	if (eye_parse_addr_port(loop->pool, &addr, ip, eye_strlen(ip)) != EYE_OK) {
		perror("eye_parse_addr_port failure\n");
		return -3;
	}

	ls = eye_create_listening(loop, addr.sockaddr, addr.socklen);
	if (NULL == ls) {
		perror("eye_create_listening failure\n");
		return -4;
	}

	// init ls
	ls->pool_size = 256;
	ls->handler = on_connected;

	eye_configure_listening_sockets(loop);

	if (eye_open_listening_sockets(loop) != EYE_OK) {
		perror("eye_open_listening_sockets failure\n");
		return -5;
	}

	if (eye_event_loop_worker_init(loop, 2) != EYE_OK) {
		perror("eye_event_loop_worker_init failure\n");
		return -5;
	}

	//eye_event_loop_start(loop);
	eye_event_loop_start_thread(loop);
	eye_event_loop_join(loop);

	return 0;
}

void on_connected(eye_net_connection_t *c)
{
	printf("on_connected\n");

	c->read->handler = on_data;
	c->write->handler = on_write;

	if (eye_handle_read_event(c->listening->loop, c->read, 0) != EYE_OK) {
		printf("eye_handle_read_event error\n");
		on_closed(c);
	}

	// c->write->ready = 0;
	// if (eye_handle_write_event(c->listening->loop, c->write) != EYE_OK) {
	// 	printf("eye_handle_write_event\n");
	// 	eye_close_connection(c);
	// }
}

void on_data(eye_event_t *ev)
{
	printf("on_data\n");

	eye_net_connection_t		*c;
	eye_event_loop_t			*loop;
	eye_pool_t					*pool;
	size_t						 size;
	eye_buf_t					*buf;
	ssize_t						 n;
	u_char						 tmp[1025];


	c = (eye_net_connection_t *) ev->data;
	loop = c->listening->loop;
	pool = c->pool;

	if (c->buffer == NULL) {
		c->buffer = eye_create_buf(pool, 1024);
	}

	buf = c->buffer;

	for ( ;; ) {

		size = buf->end - buf->last;

		if (size && ev->ready && !ev->error) {

			n = c->recv(c, buf->last, size);

			if (n == EYE_AGAIN) {
				if (eye_handle_read_event(loop, c->read, 0) != EYE_OK) {
					printf("on_data eye_handle_read_event error\n");
					on_closed(c);
					return;
				}
				printf("on_data EYE_EAGAIN\n");
				break;
			}

			if (n == EYE_ERROR) {
				printf("on_data EYE_ERROR\n");
				ev->eof = 1;
				n = 0;
			}

			if (n >= 0) {
				buf->last += n;
				continue;
			}
		}

		break;
	}


	if (buf->last - buf->pos > 0) {
		// message callback
		eye_memset(tmp, 0, sizeof(tmp));
		eye_memcpy(tmp, buf->pos, buf->last - buf->pos);
		printf("on_data recv:%s\n", tmp);
		buf->pos = buf->last = buf->start;

		n = c->send(c, tmp, eye_strlen(tmp));

		if (n == EYE_ERROR) {
			ev->eof = 1;
		}

		if (n == EYE_AGAIN) {
			if (eye_handle_write_event(loop, c->write) != EYE_OK) {
				printf("on_data eye_handle_write_event\n");
				on_closed(c);
				return;
			}
		}
	}

	if (ev->eof) {
		printf("on_data eof\n");
		on_closed(c);
	}
}

void on_write(eye_event_t *ev)
{
	printf("on_write\n");
}

void on_closed(eye_net_connection_t *c)
{
	printf("on_closed\n");
	eye_close_connection(c);
	printf("on_closed pool:%p\n", c->pool);
	eye_destroy_pool(c->pool);
}
