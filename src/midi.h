/*****************************************************************************
 * midi.h
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

#include <glib.h> 

struct _midi;
typedef struct _midi midi_t;

midi_t* midi_init(GAsyncQueue *queue);
gpointer midi_run(gpointer data);
void midi_cleanup (midi_t *midi);

#endif // __MIDI_H_
