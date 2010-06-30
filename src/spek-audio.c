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

#include "spek-audio.h"

void spek_audio_init () {
	av_register_all ();
}

SpekAudioContext * spek_audio_open (const char *file_name) {
	SpekAudioContext *cx;
	int i;

	cx = g_new0 (SpekAudioContext, 1);
	cx->file_name = g_strdup (file_name);

	if (av_open_input_file (&cx->format_context, file_name, NULL, 0, NULL) != 0) {
		/* TODO
		 */
		return cx;
	}
	if (av_find_stream_info (cx->format_context)) {
		/* TODO
		 */
		return cx;
	}
	cx->audio_stream = -1;
	for (i = 0; i < cx->format_context->nb_streams; i++) {
		if (cx->format_context->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO) {
			cx->audio_stream = i;
			break;
		}
	}
	if (cx->audio_stream == -1) {
		/* TODO
		 */
		return cx;
	}
	cx->codec_context = cx->format_context->streams[cx->audio_stream]->codec;
	cx->bit_rate = cx->codec_context->bit_rate;
	cx->sample_rate = cx->codec_context->sample_rate;
	cx->channels = cx->codec_context->channels;
	cx->codec = avcodec_find_decoder (cx->codec_context->codec_id);
	if (cx->codec == NULL) {
		/* TODO
		 */
		return cx;
	}
	if (avcodec_open (cx->codec_context, cx->codec) < 0) {
		/* TODO
		 */
		return cx;
	}

	return cx;
}

void spek_audio_close (SpekAudioContext *cx) {
	if (cx->file_name != NULL) {
		g_free (cx->file_name);
	}
	if (cx->codec_context != NULL) {
		avcodec_close (cx->codec_context);
	}
	if (cx->format_context != NULL) {
		av_close_input_file (cx->format_context);
	}
	g_free (cx);
}
