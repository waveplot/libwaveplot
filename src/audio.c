/*
 * Copyright 2014 Ben Ockmore
 *
 * This file is part of libwaveplot.

 * libwaveplot is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.

 * libwaveplot is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with libwaveplot. If not, see <http://www.gnu.org/licenses/>.
 */

#include "audio.h"

#include "info.h"
#include "file.h"

#include "libavcodec/avcodec.h"
#include "libavutil/samplefmt.h"
#include "libavresample/avresample.h"

#include <stdlib.h>
#include <stdint.h>

static char* get_error_text(const int error)
{
	static char error_buffer[255];
	av_strerror(error, error_buffer, sizeof(error_buffer));
	return error_buffer;
}

audio_samples_t* alloc_audio_samples(void)
{
	audio_samples_t* result = (audio_samples_t*)malloc(sizeof(audio_samples_t));

	if(result == NULL)
		return NULL;

	result->samples = NULL;
	return result;

}

void free_audio_samples_data(audio_samples_t* samples)
{
	if(samples->samples != NULL)
	{
		for(size_t i = 0; i != samples->num_channels; ++i)
		{
			free(samples->samples[i]);
		}
		free(samples->samples);
		samples->samples = NULL;
	}
}

void free_audio_samples(audio_samples_t* samples)
{
	free_audio_samples_data(samples);
	free(samples);
}

int get_samples(audio_samples_t* samples, file_t* file, info_t* info)
{
	int error;
	AVPacket input_packet;

	/**
	* https://github.com/libav/libav/blob/master/doc/examples/transcode_aac.c
	**/
	av_init_packet(&input_packet);
	input_packet.data = NULL;
	input_packet.size = 0;

	if((error = av_read_frame(file->format_context, &input_packet)) < 0)
	{
		if (error == AVERROR_EOF)
			return -2;
		else
		{
			fprintf(stderr, "Could not read frame (error '%s')\n",
			        get_error_text(error));
			return -1;
		}
	}

	if(input_packet.stream_index != file->stream->index)
	{
		return 0;
	}

	int decoded;
	AVCodecContext* cc = file->codec_context;
	AVFrame* frame = avcodec_alloc_frame();
	avcodec_get_frame_defaults(frame);

	if((error = avcodec_decode_audio4(file->codec_context, frame,
	                                  &decoded, &input_packet)) < 0)
	{
		fprintf(stderr, "Could not decode frame (error '%s')\n",
		        get_error_text(error));
		av_free_packet(&input_packet);
		return -1;
	}

	if(decoded != 0)
	{
		if((frame->format != cc->sample_fmt) ||
		        (frame->channel_layout != cc->channel_layout) ||
		        (frame->sample_rate != cc->sample_rate))
			return -1;
	}
	else
	{
		return 0;
	}

	// From here on, frame characteristics are known the match codec context characteristics, so we can resample based on that.

	int total_bytes = av_samples_get_buffer_size(NULL, info->num_channels, frame->nb_samples, cc->sample_fmt, 1);
	uint8_t bytes_per_sample = (uint8_t)av_get_bytes_per_sample(cc->sample_fmt);

	samples->length = total_bytes / (info->num_channels * bytes_per_sample);
	samples->num_channels = info->num_channels;

	uint8_t** output_data = (uint8_t**)malloc(info->num_channels * sizeof(uint8_t*));

	for(size_t i = 0; i != info->num_channels; ++i)
	{
		output_data[i] = (uint8_t*)malloc(samples->length * sizeof(float));
	}

	// Use avresample to get data in planar float form, for later processing
	avresample_convert(file->resample_context, output_data, 0, samples->length, frame->data, 0, frame->nb_samples);

	free_audio_samples_data(samples);

	samples->samples = (float**)output_data;

#if (LIBAVCODEC_VERSION_MAJOR > 54)
	avcodec_free_frame(&frame);
#else
	av_free(frame);
#endif

	av_free_packet(&input_packet);

	return decoded;
}
