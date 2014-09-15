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

#include "file.h"

#include <string.h>
#include <stdlib.h>
#include "libavutil/opt.h"

static char* get_error_text(const int error)
{
	static char error_buffer[255];
	av_strerror(error, error_buffer, sizeof(error_buffer));
	return error_buffer;
}

file_t* alloc_file(void)
{
	file_t* result = (file_t*)malloc(sizeof(file_t));

	if(result == NULL)
		return NULL;

	result->path = NULL;
	result->stream = NULL;
	result->codec_context = NULL;
	result->codec = NULL;
	result->format_context = NULL;
	result->resample_context = NULL;

	return result;
}

void free_file(file_t* file)
{
	if(file->path != NULL)
		free(file->path);

	if(file->resample_context != NULL)
		avresample_free(&(file->resample_context));

	if(file->codec_context != NULL)
		avcodec_close(file->codec_context);

	if(file->format_context != NULL)
		avformat_close_input(&(file->format_context));

	free(file);
}

int load_file(char* path, file_t* file)
{
	int error;

	file->path = (char*)malloc(strlen(path)+1);
	strcpy(file->path,path);

	file->format_context = NULL;

	if(avformat_open_input(&(file->format_context), file->path, NULL, NULL) < 0)
	{
		free(file->path);
		return -1;
	}

	if(avformat_find_stream_info(file->format_context, NULL) < 0)
	{
		free(file->path);
		avformat_close_input(&file->format_context);
		return -1;
	}

	int stream_no = av_find_best_stream(file->format_context,AVMEDIA_TYPE_AUDIO,-1,-1,NULL,0);

	if(stream_no < 0)
	{
		free(file->path);
		avformat_close_input(&file->format_context);
		return -1;
	}

	file->stream = file->format_context->streams[stream_no];

	if(!(file->codec = avcodec_find_decoder(file->stream->codec->codec_id)))
	{
		fprintf(stderr, "Could not find input codec\n");
		free(file->path);
		avformat_close_input(&file->format_context);
		return -1;
	}

	if((error = avcodec_open2(file->stream->codec, file->codec, NULL)) < 0)
	{
		fprintf(stderr, "Could not open input codec (error '%s')\n",
		        get_error_text(error));
		free(file->path);
		avformat_close_input(&file->format_context);
		return -1;
	}

	file->codec_context = file->stream->codec;

	file->resample_context = avresample_alloc_context();
	av_opt_set_int(file->resample_context, "in_channel_layout",  file->codec_context->channel_layout,0);
	av_opt_set_int(file->resample_context, "out_channel_layout", file->codec_context->channel_layout,0);
	av_opt_set_int(file->resample_context, "in_sample_rate",     file->codec_context->sample_rate,   0);
	av_opt_set_int(file->resample_context, "out_sample_rate",    file->codec_context->sample_rate,   0);
	av_opt_set_int(file->resample_context, "in_sample_fmt",      file->codec_context->sample_fmt,    0);
	av_opt_set_int(file->resample_context, "out_sample_fmt",     AV_SAMPLE_FMT_FLTP,   0);

	avresample_open(file->resample_context);

	return 0;
}
