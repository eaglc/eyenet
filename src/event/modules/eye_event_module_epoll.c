

#include "eye_core.h"
#include "eye_event.h"
#include "eye_event_loop.h"


#define EYE_EVENT_LIST_SIZE 512

static eye_int_t eye_event_module_epoll_add_event(eye_event_loop_t *loop, eye_event_t *ev, eye_int_t event, eye_uint_t flags);
static eye_int_t eye_event_module_epoll_del_event(eye_event_loop_t *loop, eye_event_t *ev, eye_int_t event, eye_uint_t flags);
static eye_int_t eye_event_module_epoll_add_conn(eye_event_loop_t *loop, eye_net_connection_t *c);
static eye_int_t eye_event_module_epoll_del_conn(eye_event_loop_t *loop, eye_net_connection_t *c, eye_uint_t flags);
static eye_int_t eye_event_module_epoll_process(eye_event_loop_t *loop, eye_msec_t timer, eye_uint_t flags);
static eye_int_t eye_event_module_epoll_init(eye_event_loop_t *loop);
static eye_int_t eye_event_module_epoll_done(eye_event_loop_t *loop);

// convert from eye_x_event to epoll event.
static uint32_t eye_event_module_epoll_flags(eye_uint_t flags);


struct eye_event_actions_s eye_event_actions = 
{
	eye_event_module_epoll_add_event,
	eye_event_module_epoll_del_event,
	eye_event_module_epoll_add_event,
	eye_event_module_epoll_del_event,
	eye_event_module_epoll_add_conn,
	eye_event_module_epoll_del_conn,
	eye_event_module_epoll_process,
	eye_event_module_epoll_init,
	eye_event_module_epoll_done
};


struct eye_event_module_epoll_ctx
{
	int 					epfd;
	struct epoll_event 		event_list[EYE_EVENT_LIST_SIZE];
};
struct eye_event_module_epoll_ctx  *eye_event_module_epoll_ctx_init();
struct epoll_event 				   *eye_event_module_epoll_ctx_event_list(struct eye_event_module_epoll_ctx *ctx);
int 								eye_event_module_epoll_ctx_epfd(struct eye_event_module_epoll_ctx *ctx);
void 								eye_event_module_epoll_ctx_destroy(struct eye_event_module_epoll_ctx *ctx);


static uint32_t eye_event_module_epoll_flags(eye_uint_t flags)
{
	uint32_t evs = 0;

	if (flags & EYE_EVENT_CLEAR) {
		evs |= EPOLLET;
	}

	if (flags & EYE_EVENT_ONESHOT) {
		evs |= EPOLLONESHOT;
	}

	return evs;
}

static eye_int_t eye_event_module_epoll_init(eye_event_loop_t *loop)
{
	struct eye_event_module_epoll_ctx *ctx;

	ctx = eye_event_module_epoll_ctx_init();

	if (ctx == NULL) {
		return EYE_ERROR;
	}

	loop->module_ctx = (void *)ctx;

	eye_event_flags = EYE_USE_CLEAR_EVENT;

	return EYE_OK;
}

static eye_int_t eye_event_module_epoll_process(eye_event_loop_t *loop, eye_msec_t timer, eye_uint_t flags)
{
	int 					i;
	int 					events;
	int 					epfd;
	uint32_t				revents;
	struct epoll_event 		*event_list;
	eye_err_t 				err;
	eye_event_t 			*rev;
	eye_event_t 			*wev;
	eye_net_connection_t 	*c;

	epfd = eye_event_module_epoll_ctx_epfd((struct eye_event_module_epoll_ctx *) loop->module_ctx);
	event_list = eye_event_module_epoll_ctx_event_list((struct eye_event_module_epoll_ctx *) loop->module_ctx);

	events = epoll_wait(epfd, event_list, EYE_EVENT_LIST_SIZE, timer);

	err = (events == -1) ? eye_errno : 0;

	if (err) {
		if (err == EYE_EINTR) {
			return EYE_OK;
		}

		return EYE_ERROR;
	}

	if (events == 0) {
		return EYE_OK;
	}

	for (i = 0; i < events; i++) {
		c = event_list[i].data.ptr;
		rev = c->read;

		if (c->fd == -1) {
			continue;
		}

		revents = event_list[i].events;

		if (revents & (EPOLLERR|EPOLLHUP)) {
			revents |= EPOLLIN|EPOLLOUT;
		}

		// todo: support post event.
		if ((revents & EPOLLIN) && rev->active) {

			if (revents & EPOLLRDHUP) {
                rev->pending_eof = 1;
			}
            rev->available = 1;
            rev->ready = 1;

			rev->handler(rev);
		}

		if (c->fd == -1) {
			continue;
		}

		wev = c->write;

        wev->ready = 1;

		if ((revents & EPOLLOUT) && wev->active) {
			wev->handler(wev);
		}
	}

	return EYE_OK;
}

static eye_int_t eye_event_module_epoll_add_event(eye_event_loop_t *loop, eye_event_t *ev, eye_int_t event, eye_uint_t flags)
{
	int 					op;
	uint32_t 				events, prev;
	int 					epfd;
	eye_net_connection_t	   	*c;
	eye_event_t 			*e;
	struct epoll_event 		ee;

	epfd = eye_event_module_epoll_ctx_epfd((struct eye_event_module_epoll_ctx *)loop->module_ctx);
	c = ev->data;

	if (event == EYE_EVENT_READ) {
		e = c->write;
		prev = EPOLLOUT;
		events = EPOLLIN|EPOLLRDHUP;
	} else {
		e = c->read;
		prev = EPOLLIN|EPOLLRDHUP;
		events = EPOLLOUT;
	}

	if (e->active) {
		op = EPOLL_CTL_MOD;
		events |= prev;
	} else {
		op = EPOLL_CTL_ADD;
	}

	ee.events = events | eye_event_module_epoll_flags(flags);
	ee.data.ptr = (void *)c;

	if (epoll_ctl(epfd, op, c->fd, &ee) == -1) {
		return EYE_ERROR;
	}

	ev->active = 1;

	return EYE_OK;
}

static eye_int_t eye_event_module_epoll_del_event(eye_event_loop_t *loop, eye_event_t *ev, eye_int_t event, eye_uint_t flags)
{
	int 					op;
	uint32_t				prev;
	int 					epfd;
	eye_event_t 			*e;
	eye_net_connection_t 	*c;
	struct epoll_event 		ee;

	if (flags & EYE_EVENT_CLOSE) {
		ev->active = 0;
		return EYE_OK;
	}

	epfd = eye_event_module_epoll_ctx_epfd((struct eye_event_module_epoll_ctx *)loop->module_ctx);
	c = ev->data;

	if (event == EYE_EVENT_READ) {
		e = c->write;
		prev = EPOLLOUT;
	} else {
		e = c->read;
		prev = EPOLLIN|EPOLLRDHUP;
	}

	if (e->active) {
		op = EPOLL_CTL_MOD;
		ee.events = prev | eye_event_module_epoll_flags(flags);
		ee.data.ptr = (void *)c;
	} else {
		op = EPOLL_CTL_DEL;
		ee.events = 0;
		ee.data.ptr = NULL;
	}

	if (epoll_ctl(epfd, op, c->fd, &ee) == -1) {
		return EYE_ERROR;
	}

	ev->active = 0;

	return EYE_OK;
}

static eye_int_t eye_event_module_epoll_add_conn(eye_event_loop_t *loop, eye_net_connection_t *c)
{
	struct epoll_event 	ee;
	int 				epfd;

	epfd = eye_event_module_epoll_ctx_epfd((struct eye_event_module_epoll_ctx *)loop->module_ctx);

	ee.events = EPOLLIN|EPOLLOUT|EPOLLET|EPOLLRDHUP;
	ee.data.ptr = (void *)c;

	if (epoll_ctl(epfd, EPOLL_CTL_ADD, c->fd, &ee) == -1) {
		return EYE_ERROR;
	}

	c->read->active = 1;
	c->write->active = 1;

	return EYE_OK;
}

static eye_int_t eye_event_module_epoll_del_conn(eye_event_loop_t *loop, eye_net_connection_t *c, eye_uint_t flags)
{
	struct epoll_event 	ee;
	int 				epfd;

	if (flags & EYE_EVENT_CLOSE) {
		c->read->active = 0;
		c->write->active = 0;
		return EYE_OK;
	}

	epfd = eye_event_module_epoll_ctx_epfd((struct eye_event_module_epoll_ctx *)loop->module_ctx);

	ee.events = 0;
	ee.data.ptr = NULL;

	if (epoll_ctl(epfd, EPOLL_CTL_DEL, c->fd, &ee) == -1) {
		return EYE_ERROR;
	}

	c->read->active = 0;
	c->write->active = 0;

	return EYE_OK;
}

static eye_int_t eye_event_module_epoll_done(eye_event_loop_t *loop)
{
	eye_event_module_epoll_ctx_destroy((struct eye_event_module_epoll_ctx *)loop->module_ctx);
	loop->module_ctx = NULL;

	return EYE_OK;
}

struct eye_event_module_epoll_ctx *eye_event_module_epoll_ctx_init()
{
	struct eye_event_module_epoll_ctx *ctx;
	ctx = (struct eye_event_module_epoll_ctx *)eye_alloc(sizeof(struct eye_event_module_epoll_ctx));
	memset(ctx, 0, sizeof(struct eye_event_module_epoll_ctx));

	ctx->epfd = -1;

	ctx->epfd = epoll_create1(EPOLL_CLOEXEC);

	if (ctx->epfd == -1) {
		// log error
		eye_event_module_epoll_ctx_destroy(ctx);
		return NULL;
	}

	return ctx;
}

struct epoll_event *eye_event_module_epoll_ctx_event_list(struct eye_event_module_epoll_ctx *ctx)
{
	return ctx->event_list;
}

int eye_event_module_epoll_ctx_epfd(struct eye_event_module_epoll_ctx *ctx)
{
	return ctx->epfd;
}

void eye_event_module_epoll_ctx_destroy(struct eye_event_module_epoll_ctx *ctx)
{
	if (ctx->epfd != -1) {
		close(ctx->epfd);
	}

	eye_free(ctx);
}


