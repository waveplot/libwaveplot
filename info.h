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

#ifndef __LIBWP_INFO_H__
#define __LIBWP_INFO_H__

#include <stdint.h>

#include "typedefs.h"

struct info_t_t
{
    uint32_t duration_secs;

    uint8_t num_channels;

    uint16_t bit_depth;
    uint32_t bit_rate;
    uint32_t sample_rate;

    char* file_format;
};

/* For allocating and freeing information structures */
info_t* alloc_info(void);
void free_info(info_t* info);

void get_info(info_t* info, file_t* file);

#endif //__LIBWP_INFO_H__
