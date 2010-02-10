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

#include <stdio.h>
#include <malloc.h>
#include <glib.h>

struct _midi_thread {
	GAsyncQueue *queue;
};

midi_thread_t*
midi_init (GAsyncQueue *queue)
{
	midi_thread_t *midi = calloc (1, sizeof (midi_thread_t));
	
	if (!midi) return NULL;
	midi->queue = g_async_queue_ref (queue);
	
	return midi;
}

void
midi_cleanup (midi_thread_t *midi)
{
	if (midi->queue)
		g_async_queue_unref (midi->queue);
	free (midi);
	return;
}

gpointer
midi_run (gpointer data)
{
	midi_thread_t *midi = (midi_thread_t*) data;
	printf ("sink thread running \n");

	for(;;) {
		g_async_queue_pop (midi->queue);
		printf ("Got a message !");
	}
	return NULL;
}
