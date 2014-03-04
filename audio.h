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

#include <string.h>

typedef struct file_t_t file_t;

typedef struct info_t_t info_t;

typedef struct audio_samples_t_t
{
    float** samples;
    size_t length;
} audio_samples_t;

audio_samples_t* alloc_audio_samples(void);
void free_audio_samples(audio_samples_t* file);
audio_samples_t* get_samples(audio_samples_t* samples, file_t* file, info_t* info);
