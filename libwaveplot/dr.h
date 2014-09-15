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

#ifndef __LIBWP_DR_H__
#define __LIBWP_DR_H__

#include <stdlib.h>

#include "dllexport.h"
#include "typedefs.h"

struct dr_t_t
{
	float** channel_peak;
	float** channel_rms;
	size_t num_channels;
	size_t length;

	float rating;

	size_t _capacity;
	size_t _processed_samples;
};

LIBWAVEPLOT_API dr_t* alloc_dr(void);
LIBWAVEPLOT_API void free_dr(dr_t* dr);
LIBWAVEPLOT_API void init_dr(dr_t* dr, info_t* info);
LIBWAVEPLOT_API void update_dr(dr_t* dr_data, audio_samples_t* samples, info_t* info);
LIBWAVEPLOT_API void finish_dr(dr_t* dr, info_t* info);

#endif //__LIBWP_DR_H__
