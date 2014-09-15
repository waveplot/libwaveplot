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

#include "info.h"

#include "file.h"

#include <string.h>
#include <stdlib.h>

info_t* alloc_info(void)
{
	return (info_t*)malloc(sizeof(info_t));
}

void free_info(info_t* info)
{
	free(info->file_format);
	free(info);
}

void get_info(info_t* info, file_t* file)
{
	/* Calculate the approximate duration of the stream - assumed to be less
	 * than 18 hours long. */
	info->duration_secs = (uint32_t)((file->stream->duration * file->stream->time_base.num) / file->stream->time_base.den);

	info->num_channels = (uint8_t)file->codec_context->channels;
	info->sample_rate  = (uint32_t)file->codec_context->sample_rate;
	info->bit_rate = (uint32_t)file->codec_context->bit_rate;

	enum AVSampleFormat sample_format = file->codec_context->sample_fmt;
	info->bit_depth = (uint16_t)av_get_bytes_per_sample(sample_format) * 8;

	info->file_format = (char*)malloc(strlen(file->codec->name)+1);
	strcpy(info->file_format,file->codec->name);
}
