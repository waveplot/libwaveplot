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

#ifndef __LIBWP_FILE_H__
#define __LIBWP_FILE_H__

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavresample/avresample.h"

#include "typedefs.h"

struct file_t_t
{
	char* path;

	AVStream* stream;
	AVCodecContext* codec_context;
	AVCodec* codec;
	AVFormatContext* format_context;
	AVAudioResampleContext* resample_context;

};

/* Initialize WavePlot and libav */
void init(void);

/* Allocate storage for a file structure, and deallocate. */
file_t* alloc_file(void);
void free_file(file_t*);

/* Load a file with libav, into an allocated file structure. */
int load_file(char*, file_t*);

#endif //__LIBWP_FILE_H__
