/*****************************************************************************
 * message.h: Message passing API
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

#ifndef __MESSAGE_H_
#define __MESSAGE_H_

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <glib.h>

typedef enum {
	MSG_TYPE_CONTROL,
	MSG_TYPE_EVENT
} message_type_t;

typedef enum {
	CONTROL_ACTION_SHUTDOWN,
	CONTROL_ACTION_DISCONNECT,
	CONTROL_ACTION_RECONNECT,
	CONTROL_ACTION_RESET,
	CONTROL_ACTION_SET_CHANNEL
} control_action_t;

typedef enum {
	EVENT_TYPE_TOUCH,
	EVENT_TYPE_RELEASE,
	EVENT_TYPE_MOVE
} event_type_t;

typedef struct {
	message_type_t type;
	gpointer data;
	gint refcount;
	GMutex *lock;
} message_t;

typedef struct {
	message_t* msg;
	control_action_t action;
	gpointer user_data;
} control_message_t;

typedef struct {
	message_t* msg;
	event_type_t type;
	gint pos_x;
	gint pos_y;
} event_message_t;

gboolean send_shutdown_control (GAsyncQueue *queue);
gboolean send_disconnect_control (GAsyncQueue *queue);
gboolean send_reconnect_control (GAsyncQueue *queue);
gboolean send_reset_control (GAsyncQueue *queue);
gboolean send_set_channel_control (gint channel, GAsyncQueue *queue);

gboolean send_event (event_type_t type, gint x, gint y, GAsyncQueue *queue);

message_t* message_new (message_type_t type);
message_t* message_incref (message_t *msg);
void message_decref (message_t *msg);
void message_destroy (message_t *msg);

#endif // __MESSAGE_H_
