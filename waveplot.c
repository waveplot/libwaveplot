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

#include "waveplot.h"

#include "audio.h"
#include "info.h"

#include <string.h>
#include <stdint.h>
#include <math.h>

waveplot_t* alloc_waveplot(void)
{
	waveplot_t* result = (waveplot_t*)malloc(sizeof(waveplot_t));

	if(result == NULL)
		return NULL;

	result->length = 0;

	/* Assume song likely to be less than four minutes
	 * 4 mins * 60 secs = 240 secs, 240 secs * 4 Hz = 960 chunks
	 */
	result->_capacity = 1024;
	result->values = (float*)malloc(sizeof(float) * result->_capacity);
	return result;

}

void free_waveplot(waveplot_t* waveplot)
{
	free(waveplot->values);
	free(waveplot);
}

void update_waveplot(waveplot_t* waveplot, audio_samples_t* samples, info_t* info)
{
	static uint32_t current_chunk_samples = 0;
	static float current_chunk = 0.0;
	uint32_t samples_per_chunk = (info->sample_rate / 4);

	// Check whether there are too many samples for the current chunk
	if((samples->length + current_chunk_samples) > samples_per_chunk)
	{
		// New chunk - check whether the container needs a resize
		if(waveplot->length == waveplot->_capacity)
		{
			// Increase the length of the container by 2, and copy the old data
			waveplot->_capacity *= 2;
			float* new_array = (float*)malloc(sizeof(float) * waveplot->_capacity);
			memcpy(new_array, waveplot->values, sizeof(float) * waveplot->length);
			free(waveplot->values);
			waveplot->values = new_array;
		}
	}

	for(size_t i = 0; i != samples->length; ++i)
	{
		float combined_channels = 0.0f;
		for(size_t channel = 0; channel != info->num_channels; ++channel)
		{
			combined_channels += fabs(samples->samples[channel][i]);
		}

		if(current_chunk_samples == samples_per_chunk)
		{
			waveplot->values[waveplot->length] = current_chunk;
			++waveplot->length;

			current_chunk = 0.0;
			current_chunk_samples = 0;
		}

		current_chunk += combined_channels;
		++current_chunk_samples;
	}
}

void finish_waveplot(waveplot_t* waveplot)
{
	static const float sample_weightings[] = {10.0, 8.0, 5.0, 3.0};
	static const size_t num_weightings = sizeof(sample_weightings)/sizeof(float);
	float* processed = (float*)malloc(sizeof(float) * waveplot->_capacity);

	for(size_t i = 0; i != waveplot->length; ++i)
	{
		processed[i] = 0.0f;
	}

	for(size_t i = 0; i != waveplot->length; ++i)
	{
		float chunk = waveplot->values[i];
		processed[i] += chunk * sample_weightings[0];
		for(size_t j = 1; j != num_weightings; ++j)
		{
			float value = chunk * sample_weightings[j];
			size_t index = i + j;

			if(index < waveplot->length)
			{
				processed[index] += value;
			}
			index = i - j;

			if(index < i)  //Will be greater than center if index has wrapped around to ULONG_MAX.
			{
				processed[index] += value;
			}
		}
	}

	float max_chunk = 0.0f;
	for(size_t i = 0; i != waveplot->length; ++i)
	{
		max_chunk = fmaxf(processed[i], max_chunk);
	}


	for(size_t i = 0; i != waveplot->length; ++i)
	{
		processed[i] /= max_chunk;
	}

	free(waveplot->values);
	waveplot->values = processed;
}
