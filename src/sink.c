/*****************************************************************************
 * sink.c: Event sink
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

#include "sink.h"
#include "message.h"

#include <stdio.h>
#include <glib.h>

struct _sink {
	GAsyncQueue *queue;
	GHashTable *control_handlers;
	GHashTable *event_handlers;
	gboolean die;
};

typedef struct {
	msg_handler_func func;
	gpointer data;
} msg_handler_t;

static void handle_control_msg (sink_t *sink, message_t *msg);
static void handle_event_msg (sink_t *sink, message_t *msg);

static void shutdown (gpointer msg, gpointer data);

sink_t*
sink_init (GAsyncQueue *queue)
{
	sink_t *sink = g_malloc0 (sizeof (sink_t));
	
	if (!sink) return NULL;

	sink->queue = g_async_queue_ref (queue);
	sink->control_handlers =
		g_hash_table_new_full (g_int_hash, g_int_equal, g_free, g_free);
	sink->event_handlers =
		g_hash_table_new_full (g_int_hash, g_int_equal, g_free, g_free);
	sink->die = FALSE;

	sink_register_control_msg_handler (sink,
		CONTROL_ACTION_SHUTDOWN, shutdown, sink);

	return sink;
}

void
sink_cleanup (sink_t *sink)
{
	if (sink->queue)
		g_async_queue_unref (sink->queue);

	if (sink->control_handlers)
		g_hash_table_destroy (sink->control_handlers);

	if (sink->event_handlers)
		g_hash_table_destroy (sink->event_handlers);

	g_free (sink);
	return;
}

gpointer
sink_run (gpointer data)
{
	message_t *msg = NULL;
	sink_t *sink = (sink_t*) data;

	printf ("sink thread running \n");

	for(;;) {
		msg = (message_t*) g_async_queue_pop (sink->queue);

		switch (msg->type) {
			case MSG_TYPE_CONTROL:
				handle_control_msg (sink, msg);
				break;
			case MSG_TYPE_EVENT:
				handle_event_msg (sink, msg);
				break;
		}

		message_decref (msg);
		if (sink->die) break;
	}

	printf ("sink thread shutting down \n");
	return NULL;
}

static void
shutdown (gpointer msg, gpointer data)
{
	(void) msg;
	sink_t* sink = (sink_t*) data;
	sink->die = TRUE;
}

void
sink_register_control_msg_handler (sink_t *sink,
	control_action_t action, msg_handler_func func, gpointer data)
{
	gint *key = g_malloc0 (sizeof (gint));
	msg_handler_t* handler = g_malloc0 (sizeof (msg_handler_t));
	handler->func = func;
	handler->data = data;

	*key = action;

	g_hash_table_insert (sink->control_handlers, key, handler);
}

static void
handle_control_msg (sink_t *sink, message_t *msg)
{
	msg_handler_t *handler = NULL;
	control_message_t *cm = (control_message_t*) msg->data;
	printf ("Got a control msg: action %d \n", cm->action);

	handler = g_hash_table_lookup (sink->control_handlers, &cm->action);

	if (handler)
		handler->func (cm, handler->data);
}

static void
handle_event_msg (sink_t *sink, message_t *msg)
{
	return;
}
