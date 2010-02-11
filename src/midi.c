/*****************************************************************************
 * midi.c: Midi subsystem
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

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "midi.h"
#include "message.h"

#include <stdio.h>
#include <glib.h>

struct _midi {
	GAsyncQueue *queue;
	GHashTable *control_handlers;
	GHashTable *event_handlers;
	gboolean die;
};

typedef struct {
	msg_handler_func func;
	gpointer data;
} msg_handler_t;

static void handle_control_msg (midi_t *midi, message_t *msg);
static void handle_event_msg (midi_t *midi, message_t *msg);

static void shutdown (gpointer msg, gpointer data);

midi_t*
midi_init (GAsyncQueue *queue)
{
	midi_t *midi = g_malloc0 (sizeof (midi_t));
	
	if (!midi) return NULL;

	midi->queue = g_async_queue_ref (queue);
	midi->control_handlers =
		g_hash_table_new_full (g_int_hash, g_int_equal, g_free, g_free);
	midi->event_handlers =
		g_hash_table_new_full (g_int_hash, g_int_equal, g_free, g_free);
	midi->die = FALSE;

	midi_register_control_msg_handler (midi,
		CONTROL_ACTION_SHUTDOWN, shutdown, midi);

	return midi;
}

void
midi_cleanup (midi_t *midi)
{
	if (midi->queue)
		g_async_queue_unref (midi->queue);

	if (midi->control_handlers)
		g_hash_table_destroy (midi->control_handlers);

	if (midi->event_handlers)
		g_hash_table_destroy (midi->event_handlers);

	g_free (midi);
	return;
}

gpointer
midi_run (gpointer data)
{
	message_t *msg = NULL;
	midi_t *midi = (midi_t*) data;

	printf ("sink thread running \n");

	for(;;) {
		msg = (message_t*) g_async_queue_pop (midi->queue);

		switch (msg->type) {
			case MSG_TYPE_CONTROL:
				handle_control_msg (midi, msg);
				break;
			case MSG_TYPE_EVENT:
				handle_event_msg (midi, msg);
				break;
		}

		message_decref (msg);
		if (midi->die) break;
	}

	printf ("sink thread shutting down \n");
	return NULL;
}

static void
shutdown (gpointer msg, gpointer data)
{
	(void) msg;
	midi_t* midi = (midi_t*) data;
	midi->die = TRUE;
}

void
midi_register_control_msg_handler (midi_t *midi,
	control_action_t action, msg_handler_func func, gpointer data)
{
	gint *key = g_malloc0 (sizeof (gint));
	msg_handler_t* handler = g_malloc0 (sizeof (msg_handler_t));
	handler->func = func;
	handler->data = data;

	*key = action;

	g_hash_table_insert (midi->control_handlers, key, handler);
}

static void
handle_control_msg (midi_t *midi, message_t *msg)
{
	msg_handler_t *handler = NULL;
	control_message_t *cm = (control_message_t*) msg->data;
	printf ("Got a control msg: action %d \n", cm->action);

	handler = g_hash_table_lookup (midi->control_handlers, &cm->action);

	if (handler)
		handler->func (cm, handler->data);
}

static void
handle_event_msg (midi_t *midi, message_t *msg)
{
	return;
}
