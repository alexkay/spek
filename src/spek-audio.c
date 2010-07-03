/* spek-audio.c
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

#include <glib/gi18n.h>

#include "spek-audio.h"

void spek_audio_init () {
	avcodec_init ();
	/* TODO: register only audio decoders */
	av_register_all ();
}

SpekAudioContext * spek_audio_open (const char *file_name) {
	SpekAudioContext *cx;
	int i;

	cx = g_new0 (SpekAudioContext, 1);
	cx->file_name = g_strdup (file_name);

	if (av_open_input_file (&cx->format_context, file_name, NULL, 0, NULL) != 0) {
		cx->error = _("Cannot open input file");
		return cx;
	}
	if (av_find_stream_info (cx->format_context) < 0) {
		/* 24-bit APE returns an error but parses the stream info just fine */
		if (cx->format_context->nb_streams <= 0) {
			cx->error = _("Cannot find stream info");
			return cx;
		}
	}
	cx->audio_stream = -1;
	for (i = 0; i < cx->format_context->nb_streams; i++) {
		if (cx->format_context->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO) {
			cx->audio_stream = i;
			break;
		}
	}
	if (cx->audio_stream == -1) {
		cx->error = _("The file contains no audio streams");
		return cx;
	}
	cx->stream = cx->format_context->streams[cx->audio_stream];
	cx->codec_context = cx->stream->codec;
	cx->codec = avcodec_find_decoder (cx->codec_context->codec_id);
	if (cx->codec == NULL) {
		cx->error = _("Cannot find decoder");
		return cx;
	}
	/* We can already fill in the stream info even if the codec won't be able to open it */
	cx->codec_name = g_strdup (cx->codec->long_name);
	cx->bit_rate = cx->codec_context->bit_rate;
	cx->sample_rate = cx->codec_context->sample_rate;
	cx->bits_per_sample = cx->codec_context->bits_per_raw_sample;
	if (!cx->bits_per_sample) {
		/* APE uses bpcs, FLAC uses bprs. */
		cx->bits_per_sample = cx->codec_context->bits_per_coded_sample;
	}
	cx->channels = cx->codec_context->channels;
	cx->duration = cx->stream->duration * av_q2d (cx->stream->time_base);
	if (cx->channels <= 0) {
		cx->error = _("No audio channels");
		return cx;
	}
	cx->buffer_size = (AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2;
	if (avcodec_open (cx->codec_context, cx->codec) < 0) {
		cx->error = _("Cannot open decoder");
		return cx;
	}
	switch (cx->codec_context->sample_fmt) {
	case SAMPLE_FMT_S16:
		cx->width = 16;
		cx->fp = FALSE;
		break;
	case SAMPLE_FMT_S32:
		cx->width = 32;
		cx->fp = FALSE;
		break;
	case SAMPLE_FMT_FLT:
		cx->width = 32;
		cx->fp = TRUE;
		break;
	case SAMPLE_FMT_DBL:
		cx->width = 64;
		cx->fp = TRUE;
		break;
	default:
		cx->error = _("Unsupported sample format");
		return cx;
	}
	av_init_packet (&cx->packet);
	cx->offset = 0;
	return cx;
}

void spek_audio_start (SpekAudioContext *cx, gint samples) {
	gint64 rate = cx->sample_rate * (gint64) cx->stream->time_base.num;
	cx->error_base = samples * (gint64) cx->stream->time_base.den;
	cx->frames_per_interval = av_rescale_rnd (
		cx->stream->duration, rate, cx->error_base, AV_ROUND_DOWN);
	cx->error_per_interval = (cx->stream->duration * rate) % cx->error_base;
}

gint spek_audio_read (SpekAudioContext *cx, guint8 *buffer) {
	gint buffer_size;
	gint len;
	gint res;

	if (cx->error) {
		return -1;
	}

	for (;;) {
		while (cx->packet.size > 0) {
			buffer_size = cx->buffer_size;
			len = avcodec_decode_audio3 (
				cx->codec_context, (int16_t *) buffer, &buffer_size, &cx->packet);
			if (len < 0) {
				/* Error, skip the frame. */
				cx->packet.size = 0;
				break;
			}
			cx->packet.data += len;
			cx->packet.size -= len;
			cx->offset += len;
			if (buffer_size <= 0) {
				/* No data yet, get more frames */
				continue;
			}
			/* We have data, return it and come back for more later */
			return buffer_size;
		}
		if (cx->packet.data) {
			cx->packet.data -= cx->offset;
			cx->packet.size += cx->offset;
			cx->offset = 0;
			av_free_packet (&cx->packet);
		}
		while ((res = av_read_frame (cx->format_context, &cx->packet)) >= 0) {
			if (cx->packet.stream_index == cx->audio_stream) {
				break;
			}
			av_free_packet (&cx->packet);
		}
		if (res < 0) {
			/* End of file or error. */
			return 0;
		}
	}
}

void spek_audio_close (SpekAudioContext *cx) {
	if (cx->file_name != NULL) {
		g_free (cx->file_name);
	}
	if (cx->codec_name != NULL) {
		g_free (cx->codec_name);
	}
	if (cx->packet.data) {
		cx->packet.data -= cx->offset;
		cx->packet.size += cx->offset;
		cx->offset = 0;
		av_free_packet (&cx->packet);
	}
	if (cx->codec_context != NULL) {
		avcodec_close (cx->codec_context);
	}
	if (cx->format_context != NULL) {
		av_close_input_file (cx->format_context);
	}
	g_free (cx);
}
