/*****************************************************************************
 * sink.h: Event sink
 *****************************************************************************
 * Copyright © 2010 Mirsal Ennaime
 * $Id$
 *
 * Authors:    Mirsal Ennaime <mirsal dot ennaime at gmail dot com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef __MIDI_H_
#define __MIDI_H_

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "message.h"

#include <glib.h> 

struct _sink;
typedef struct _sink sink_t;

typedef void (*msg_handler_func) (gpointer msg, gpointer data);

sink_t* sink_init(GAsyncQueue *queue);
gpointer sink_run(gpointer data);
void sink_cleanup (sink_t *sink);

void sink_register_control_msg_handler (sink_t *sink,
	control_action_t action,
	msg_handler_func func,
	gpointer data);

void sink_register_event_msg_handler (sink_t *sink,
	event_type_t event_type,
	msg_handler_func func,
	gpointer data);

#endif // __MIDI_H_
