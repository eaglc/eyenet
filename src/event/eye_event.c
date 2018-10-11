

#include "eye_event.h"

eye_uint_t	eye_event_flags;


eye_int_t eye_handle_read_event(eye_event_loop_t *loop, eye_event_t *rev, eye_uint_t flags)
{
	if (eye_event_flags & EYE_USE_CLEAR_EVENT) {

		if (!rev->active && !rev->ready) {
			if (eye_add_event(loop, rev, EYE_EVENT_READ, EYE_EVENT_CLEAR)
				== EYE_ERROR)
			{
				return EYE_ERROR;
			}
		}

		return EYE_OK;

	} else if (eye_event_flags & EYE_USE_LEVEL_EVENT) {

		if (!rev->active && !rev->ready) {
			if (eye_add_event(loop, rev, EYE_EVENT_READ, EYE_EVENT_LEVEL)
				== EYE_ERROR)
			{
				return EYE_ERROR;
			}

			return EYE_OK;
		}

		if (rev->active && (rev->ready || (flags & EYE_EVENT_CLOSE))) {
			if (eye_del_event(loop, rev, EYE_EVENT_READ, EYE_EVENT_LEVEL | flags)
				== EYE_ERROR)
			{
				return EYE_ERROR;
			}

			return EYE_OK;
		}
	}

	return EYE_OK;
}


eye_int_t eye_handle_write_event(eye_event_loop_t *loop, eye_event_t *wev)
{
	if (eye_event_flags & EYE_USE_CLEAR_EVENT) {
		if (!wev->active && !wev->ready) {
			printf("hehehehe \n");
			if (eye_add_event(loop, wev, EYE_EVENT_WRITE, EYE_EVENT_CLEAR)
				== EYE_ERROR)
			{
				return EYE_ERROR;
			}
		}

		return EYE_OK;

	} else if (eye_event_flags & EYE_USE_LEVEL_EVENT) {
		if (!wev->active && !wev->ready) {
			if (eye_add_event(loop, wev, EYE_EVENT_WRITE, EYE_EVENT_LEVEL)
				== EYE_ERROR)
			{
				return EYE_ERROR;
			}

			return EYE_OK;
		}

		if (wev->active && wev->ready) {
			if (eye_del_event(loop, wev, EYE_EVENT_WRITE, EYE_EVENT_LEVEL)
				== EYE_ERROR)
			{
				return EYE_ERROR;
			}

			return EYE_OK;
		}
	}

	return EYE_OK;
}

