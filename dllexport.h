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

#ifndef __LIBWP_DLLEXPORT_H__
#define __LIBWP_DLLEXPORT_H__

#ifdef WIN32
#ifdef LIBWAVEPLOT_EXPORTS
#define LIBWAVEPLOT_API __declspec(dllexport) 
#else
#define LIBWAVEPLOT_API __declspec(dllimport) 
#endif
#else
#define LIBWAVEPLOT_API
#endif

#endif //__LIBWP_DLLEXPORT_H__