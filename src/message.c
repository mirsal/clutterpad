/*****************************************************************************
 * message.c: Message passing API
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

#include "message.h"

#include <glib.h>

message_t*
message_new (message_type_t type)
{
	message_t *msg = g_malloc0 (sizeof (message_t));
	msg->lock = g_mutex_new ();
	msg->type = type;

	return message_incref (msg);
}

message_t*
message_incref (message_t *msg)
{
	g_mutex_lock (msg->lock);
	++msg->refcount;
	g_mutex_unlock (msg->lock);
	return msg;
}

void
message_decref (message_t *msg)
{
	g_mutex_lock (msg->lock);

	if (--msg->refcount <= 0) {
		g_mutex_unlock (msg->lock);
		message_destroy (msg);
		return;
	}

	g_mutex_unlock (msg->lock);
}

void
message_destroy (message_t *msg)
{
	g_mutex_free (msg->lock);
	g_free (msg->data);
	g_free (msg);
}

//TODO: handle out of memory
gboolean
send_shutdown_control (GAsyncQueue *queue)
{
	message_t *msg = message_new (MSG_TYPE_CONTROL);
	control_message_t *cm = g_malloc0 (sizeof (control_message_t));
	cm->action = CONTROL_ACTION_SHUTDOWN;
	cm->msg = msg;
	msg->data = (gpointer) cm;

	g_async_queue_push (queue, msg);
	return TRUE;
}
