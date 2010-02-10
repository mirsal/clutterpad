/*****************************************************************************
 * midi.c
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
	gboolean die;
};

static void handle_control_msg (midi_t *midi, message_t *msg);
static void handle_event_msg (midi_t *midi, message_t *msg);

midi_t*
midi_init (GAsyncQueue *queue)
{
	midi_t *midi = g_malloc0 (sizeof (midi_t));
	
	if (!midi) return NULL;
	midi->queue = g_async_queue_ref (queue);
	midi->die = FALSE;
	
	return midi;
}

void
midi_cleanup (midi_t *midi)
{
	if (midi->queue)
		g_async_queue_unref (midi->queue);
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
handle_control_msg (midi_t *midi, message_t *msg)
{
	control_message_t *cm = (control_message_t*) msg->data;
	printf ("Got a control msg: action %d \n", cm->action);
	switch (cm->action) {
		case CONTROL_ACTION_SHUTDOWN:
			midi->die = TRUE;
			return;
	}
}

static void
handle_event_msg (midi_t *midi, message_t *msg)
{
	return;
}
