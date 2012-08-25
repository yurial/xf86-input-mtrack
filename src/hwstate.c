/***************************************************************************
 *
 * Multitouch X driver
 * Copyright (C) 2008 Henrik Rydberg <rydberg@euromail.se>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 **************************************************************************/

#include <signal.h>
#include "hwstate.h"

void hwstate_init(struct HWState *s, const struct Capabilities *caps)
{
	int i;
	memset(s, 0, sizeof(struct HWState));
	for (i = 0; i < DIM_FINGER; i++)
		s->data[i].tracking_id = MT_ID_NULL;
	s->max_x = get_cap_xsize(caps);
	s->max_y = get_cap_ysize(caps);
}

static void finish_packet(struct HWState *s, const struct Capabilities *caps,
			  const struct input_event *syn)
{
	static const mstime_t ms = 1000;
	int i;
	foreach_bit(i, s->used) {
		if (!caps->has_abs[MTDEV_TOUCH_MINOR])
			s->data[i].touch_minor = s->data[i].touch_major;
		if (!caps->has_abs[MTDEV_WIDTH_MINOR])
			s->data[i].width_minor = s->data[i].width_major;
	}
	timercp(&s->evtime, &syn->time);
}

static int read_event(struct HWState *s, const struct Capabilities *caps,
                      const struct input_event *ev)
{
	switch (ev->type) {
	case EV_SYN:
		//fprintf( stderr, "EV_SYN\n" );
		switch (ev->code) {
		case SYN_REPORT:
			fprintf( stderr, "  SYN_REPORT\n" );
			finish_packet(s, caps, ev);
			return 1;
			break;
		default:
			fprintf( stderr, "  UNHANDLED: %d\n", ev->code );
		}
		break;
	case EV_KEY:
		//fprintf( stderr, "EV_KEY\n" );
		switch (ev->code) {
		case BTN_LEFT:
			fprintf( stderr, "  BTN_LEFT %d\n", ev->value );
			MODBIT(s->button, MT_BUTTON_LEFT, ev->value);
			MODBIT(s->button_changed, MT_BUTTON_LEFT, 1);
			break;
		case BTN_MIDDLE:
			fprintf( stderr, "  BTN_MIDDLE %d\n", ev->value );
			MODBIT(s->button, MT_BUTTON_MIDDLE, ev->value);
			MODBIT(s->button_changed, MT_BUTTON_MIDDLE, 1);
			break;
		case BTN_RIGHT:
			fprintf( stderr, "  BTN_RIGHT %d\n", ev->value );
			MODBIT(s->button, MT_BUTTON_RIGHT, ev->value);
			MODBIT(s->button_changed, MT_BUTTON_RIGHT, 1);
			break;
		case BTN_TOUCH:
			fprintf( stderr, "  BTN_TOUCH %d\n", ev->value );
			break;
		case BTN_TOOL_FINGER:
			fprintf( stderr, "  BTN_TOOL_FINGER %d\n", ev->value );
			break;
		case BTN_TOOL_DOUBLETAP:
			fprintf( stderr, "  BTN_TOOL_DOUBLETAP %d\n", ev->value );
			break;
		case BTN_TOOL_TRIPLETAP:
			fprintf( stderr, "  BTN_TOOL_TRIPLETAP %d\n", ev->value );
			break;
		case BTN_TOOL_QUADTAP:
			fprintf( stderr, "  BTN_TOOL_QUADTAP %d\n", ev->value );
			break;
		default:
			fprintf( stderr, "  UNHANDLED: %d\n", ev->code );
		}
		break;
	case EV_ABS:
		//fprintf( stderr, "EV_ABS\n" );
		switch (ev->code) {
		case ABS_MT_SLOT:
			fprintf( stderr, "  ABS_MT_SLOT %d\n", ev->value );
			if (ev->value >= 0 && ev->value < DIM_FINGER)
				s->slot = ev->value;
			break;
		case ABS_MT_TOUCH_MAJOR:
			fprintf( stderr, "  ABS_MT_TOUCH_MAJOR %d\n", ev->value );
			s->data[s->slot].touch_major = ev->value;
			break;
		case ABS_MT_TOUCH_MINOR:
			fprintf( stderr, "  ABS_MT_TOUCH_MINOR %d\n", ev->value );
			s->data[s->slot].touch_minor = ev->value;
			break;
		case ABS_MT_WIDTH_MAJOR:
			fprintf( stderr, "  ABS_MT_WIDTH_MAJOR %d\n", ev->value );
			s->data[s->slot].width_major = ev->value;
			break;
		case ABS_MT_WIDTH_MINOR:
			fprintf( stderr, "  ABS_MT_WIDTH_MINOR %d\n", ev->value );
			s->data[s->slot].width_minor = ev->value;
			break;
		case ABS_MT_ORIENTATION:
			fprintf( stderr, "  ABS_MT_OPERATION %d\n", ev->value );
			s->data[s->slot].orientation = ev->value;
			break;
		case ABS_MT_PRESSURE:
			fprintf( stderr, "  ABS_MT_PRESSURE %d\n", ev->value );
			s->data[s->slot].pressure = ev->value;
			break;
		case ABS_MT_POSITION_X:
			fprintf( stderr, "  ABS_MT_POSITION_X %d\n", ev->value );
			s->data[s->slot].position_x = ev->value;
			break;
		case ABS_MT_POSITION_Y:
			fprintf( stderr, "  ABS_MT_POSITION_Y %d\n", ev->value );
			s->data[s->slot].position_y = ev->value;
			break;
		case ABS_MT_TRACKING_ID:
			fprintf( stderr, "  ABS_MT_TRACKING_ID %d\n", ev->value );
			s->data[s->slot].tracking_id = ev->value;
			MODBIT(s->used, s->slot, ev->value != MT_ID_NULL);
			break;
		case ABS_X:
			fprintf( stderr, "  ABS_X %d\n", ev->value );
			break;
		case ABS_Y:
			fprintf( stderr, "  ABS_Y %d\n", ev->value );
			break;
		case ABS_PRESSURE:
			fprintf( stderr, "  ABS_PRESSURE %d\n", ev->value );
			break;
		case ABS_TOOL_WIDTH:
			fprintf( stderr, "  ABS_TOOL_WIDTH %d\n", ev->value );
			break;
		default:
			fprintf( stderr, "  UNHANDLED: %d\n", ev->code );
			break;
		}
		MODBIT(s->used_changed, s->slot, 1);
		break;
	default:
		fprintf( stderr, "UNHANDLED: %d\n", ev->type );
		break;
	}
	return 0;
}

int hwstate_modify(struct HWState *s, struct mtdev *dev, int fd,
		   const struct Capabilities *caps)
{
	struct input_event ev;
	int ret;
	while ((ret = mtdev_get(dev, fd, &ev, 1)) > 0) {
		if (read_event(s, caps, &ev))
			return 1;
	}
	return ret;
}

int find_finger(const struct HWState *s, int tracking_id) {
	int i;
	foreach_bit(i, s->used) {
		if (s->data[i].tracking_id == tracking_id)
			return i;
	}
	return -1;
}
