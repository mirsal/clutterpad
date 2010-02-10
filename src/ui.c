/*****************************************************************************
 * main.c
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

#include "ui.h"

#include <stdio.h>
#include <malloc.h>
#include <clutter/clutter.h>

#define BLOB_SIZE 50

struct _ui_thread {
	GAsyncQueue *queue;
	ClutterActor *stage;
	ClutterColor *stage_color;
};

typedef struct {
	ClutterActor* blob;
	ui_thread_t *ui;
} ctx_t;

static gboolean on_touch(ClutterStage*, ClutterEvent*, gpointer);
static gboolean on_move(ClutterStage*, ClutterEvent*, gpointer);
static gboolean on_release(ClutterStage*, ClutterEvent*, gpointer);

static gboolean
on_touch (ClutterStage *stage, ClutterEvent *event, gpointer data)
{
	gfloat x = 0, y = 0;
	ctx_t *ctx = calloc (1, sizeof (ctx_t));
	ClutterColor blob_color = { 0xff, 0x00, 0x00, 0xff };

	clutter_event_get_coords (event, &x, &y);
	
	ctx->ui = (ui_thread_t*) data;
	ctx->blob = clutter_rectangle_new_with_color (&blob_color);
	clutter_actor_set_size (ctx->blob, BLOB_SIZE, BLOB_SIZE);
	clutter_actor_set_position (ctx->blob,
		(gint) x - BLOB_SIZE / 2, (gint) y - BLOB_SIZE / 2);
	clutter_container_add_actor (CLUTTER_CONTAINER (stage), ctx->blob);
	clutter_actor_show (ctx->blob);

	g_signal_connect (clutter_actor_get_stage (ctx->blob),
		"motion-event", G_CALLBACK (on_move), ctx);
	g_signal_connect (clutter_actor_get_stage (ctx->blob),
		"button-release-event", G_CALLBACK (on_release), ctx);

	g_async_queue_push (ctx->ui->queue, 42);

	return TRUE;
}

static gboolean
on_move (ClutterStage *stage, ClutterEvent *event, gpointer data)
{
	gfloat x = 0, y = 0;
	ctx_t* ctx = (ctx_t*) data;
	clutter_event_get_coords (event, &x, &y);
	clutter_actor_set_position (ctx->blob,
		(gint) x - BLOB_SIZE / 2, (gint) y - BLOB_SIZE / 2);

	return TRUE;
}

#define disconnect(ctx, func) \
	g_signal_handlers_disconnect_by_func \
                (clutter_actor_get_stage (ctx->blob), G_CALLBACK (func), ctx)

static gboolean
on_release (ClutterStage *stage, ClutterEvent *event, gpointer data)
{
	ctx_t* ctx = (ctx_t*) data;
	disconnect (ctx, on_move);
	disconnect (ctx, on_release);
	clutter_actor_destroy (ctx->blob);
	free(ctx);
	return TRUE;
}

#undef disconnect

ui_thread_t*
ui_init (GAsyncQueue *queue, int *argc, char ***argv)
{
	ClutterColor stage_color = { 0x00, 0x00, 0x00, 0xff };
	ui_thread_t *ui = calloc (1, sizeof (ui_thread_t));
	
	if (!ui) return NULL;
	
	clutter_threads_init ();
	clutter_init (argc, argv);
	ui->stage_color = clutter_color_copy (&stage_color); 
	ui->queue = g_async_queue_ref (queue);
	return ui;
}

void
ui_cleanup (ui_thread_t *ui)
{
	if (ui->stage_color)
		clutter_color_free (ui->stage_color);
	free (ui);
	return;
}

void
ui_run (ui_thread_t *ui)
{
	ui->stage = clutter_stage_get_default ();
	clutter_actor_set_size (ui->stage, 800, 600);
	clutter_stage_set_color (CLUTTER_STAGE (ui->stage), ui->stage_color);

	g_signal_connect (ui->stage, "button-press-event",
		G_CALLBACK (on_touch), ui);

	clutter_actor_show (ui->stage);
	clutter_main ();
	return;
}
