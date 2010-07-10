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
	/* Internal data */
	AVFormatContext *format_context;
	gint audio_stream;
	AVCodecContext *codec_context;
	AVStream *stream;
	AVCodec *codec;
	gint buffer_size;
	AVPacket packet;
	gint offset;

	/* Exposed properties */
	gchar *file_name;
	gchar *codec_name;
	gchar *error;
	gint bit_rate;
	gint sample_rate;
	gint bits_per_sample;
	gint width; /* number of bits used to store a sample */
	gboolean fp; /* floating-point sample representation */
	gint channels;
	gdouble duration;
	guint8 *buffer;
	gint64 frames_per_interval;
	gint64 error_per_interval;
	gint64 error_base;
} SpekAudioContext;

/* Initialise FFmpeg, should be called once on start up */
void spek_audio_init ();

/* Open the file, check if it has an audio stream which can be decoded.
 * On error, initialises the `error` field in the returned context.
 */
SpekAudioContext * spek_audio_open (const gchar *file_name);

/* Prepare the context for reading audio samples. */
void spek_audio_start (SpekAudioContext *cx, gint samples);

/* Read and decode the opened audio stream.
 * Returns -1 on error, 0 if there's nothing left to read
 * or the number of bytes decoded into the buffer.
 */
gint spek_audio_read (SpekAudioContext *cx);

/* Closes the file opened with spek_audio_open,
 * frees all allocated buffers and the context
 */
void spek_audio_close (SpekAudioContext *cx);

#endif
