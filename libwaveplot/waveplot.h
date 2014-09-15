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

#ifndef __LIBWP_WAVEPLOT_H__
#define __LIBWP_WAVEPLOT_H__

#include <stdlib.h>

#include "dllexport.h"
#include "typedefs.h"

struct waveplot_t_t
{
	float* values;
	float* resample;
	size_t length;

	size_t _capacity;
	size_t _current_chunk_samples;
	float _current_chunk;
};


/* Initialize WavePlot and libav */
LIBWAVEPLOT_API void init(void);

/* Get version information */
LIBWAVEPLOT_API const char* version(void);

LIBWAVEPLOT_API waveplot_t* alloc_waveplot(void);
LIBWAVEPLOT_API void free_waveplot(waveplot_t* waveplot);
LIBWAVEPLOT_API void update_waveplot(waveplot_t* waveplot, audio_samples_t* samples, info_t* info);
LIBWAVEPLOT_API void finish_waveplot(waveplot_t* waveplot);
LIBWAVEPLOT_API void resample_waveplot(waveplot_t* waveplot, size_t target_length, size_t target_amplitude);

LIBWAVEPLOT_API unsigned int generate_sonic_hash(waveplot_t* waveplot);

#endif //__LIBWP_WAVEPLOT_H__
