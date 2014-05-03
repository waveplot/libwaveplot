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

#include "typedefs.h"

struct waveplot_t_t
{
	float* values;
	size_t length;

	size_t _capacity;
};


waveplot_t* alloc_waveplot(void);
void free_waveplot(waveplot_t* waveplot);
void update_waveplot(waveplot_t* waveplot, audio_samples_t* samples, info_t* info);
void finish_waveplot(waveplot_t* waveplot);

#endif //__LIBWP_WAVEPLOT_H__
