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

void init(void)
{
    av_register_all();
}

file_t* alloc_file(void)
{
    return (file_t*)malloc(sizeof(file_t));
}

void free_file(file_t* file)
{
    free(file);
}

int load_file(char* path, file_t* file)
{
    av_log_set_level(AV_LOG_ERROR);

    file->path = (char*)malloc(strlen(path)+1);
    strcpy(file->path,path);

    file->format_context = NULL;

    if(avformat_open_input(&(file->format_context), file->path, NULL, NULL) < 0)
        return -1;

    if(avformat_find_stream_info(file->format_context, NULL) < 0)
        return -1;

    int stream_no = av_find_best_stream(file->format_context,AVMEDIA_TYPE_AUDIO,-1,-1,NULL,0);

    if(stream_no < 0)
        return -1;

    file->stream = file->format_context->streams[stream_no];

    file->codec_context = file->stream->codec;

    file->codec = avcodec_find_decoder(file->codec_context->codec_id);

    return 0;
}
