/* spek-audio.h
 *
 * Copyright (C) 2010  Alexander Kojevnikov <alexander@kojevnikov.com>
 *
 * Spek is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Spek is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Spek.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SPEK_AUDIO_H__
#define __SPEK_AUDIO_H__

#include <glib.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

typedef struct {
	AVFormatContext *format_context;
	gint audio_stream;
	AVCodecContext *codec_context;
	AVCodec *codec;

	/* Exposed properties
	 */
	gchar *file_name;
	gint bit_rate;
	gint sample_rate;
	gint channels;
} SpekAudioContext;

void spek_audio_init ();
SpekAudioContext * spek_audio_open (const char *file_name);
void spek_audio_close (SpekAudioContext *cx);

#endif
