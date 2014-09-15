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

#include "libavformat/avformat.h"

#ifdef min
#undef min
#endif

#define min(a, b) ((a) < (b) ? (a) : (b))

#define WAVEPLOT_RESOLUTION 200.0f

void init(void)
{
	av_register_all();

	av_log_set_level(AV_LOG_ERROR);
}

const char* version(void)
{
	return "DAMSON";
}

waveplot_t* alloc_waveplot(void)
{
	waveplot_t* result = (waveplot_t*)malloc(sizeof(waveplot_t));

	if(result == NULL)
		return NULL;

	result->length = 0;
	result->resample = NULL;
	result->_current_chunk_samples = 0;
	result->_current_chunk = 0.0f;

	/* Assume song likely to be less than four minutes
	 * 4 mins * 60 secs = 240 secs, 240 secs * 4 Hz = 960 chunks
	 */
	result->_capacity = 1024;
	result->values = (float*)malloc(sizeof(float) * result->_capacity);
	return result;

}

void free_waveplot(waveplot_t* waveplot)
{
	if(waveplot->resample != NULL)
		free(waveplot->resample);

	free(waveplot->values);
	free(waveplot);
}

void update_waveplot(waveplot_t* waveplot, audio_samples_t* samples, info_t* info)
{
	uint32_t samples_per_chunk = (info->sample_rate / 4);

	// Check whether there are too many samples for the current chunk
	if((samples->length + waveplot->_current_chunk_samples) > samples_per_chunk)
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

		if(waveplot->_current_chunk_samples == samples_per_chunk)
		{
			waveplot->values[waveplot->length] = waveplot->_current_chunk;
			++waveplot->length;

			waveplot->_current_chunk = 0.0;
			waveplot->_current_chunk_samples = 0;
		}

		waveplot->_current_chunk += combined_channels;
		++waveplot->_current_chunk_samples;
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

void resample_waveplot(waveplot_t* waveplot, size_t target_length, size_t target_amplitude)
{
	float resample_factor = (float)(waveplot->length)/target_length;

	if(waveplot->resample != NULL)
		free(waveplot->resample);

	waveplot->resample = (float*)malloc(target_length*sizeof(float));

	if(waveplot->resample == NULL)
		return;

	//printf("%f\n",resample_factor);

	//Check whether it's a down-sample
	if(resample_factor > 1.0f)
	{
		float end_frac = 0.0f, end = 0.0f;
		for(size_t i = 0; i != target_length; ++i)
		{
			float start_frac = end_frac;
			float start = end;
			end_frac = modff(resample_factor * (i+1), &end);

			float value = floorf(WAVEPLOT_RESOLUTION * waveplot->values[(size_t)start]);

			waveplot->resample[i] = value * (1.0f-start_frac);

			for(size_t j = (size_t)start + 1; j != end; ++j)
			{
				value = floorf(WAVEPLOT_RESOLUTION * waveplot->values[j]);
				waveplot->resample[i] += value;
			}

			if((size_t)end != waveplot->length)
			{
				value = floorf(WAVEPLOT_RESOLUTION * waveplot->values[(size_t)end]);
				waveplot->resample[i] += value * end_frac;
			}

			waveplot->resample[i] /= resample_factor;
		}
	}
	else
	{
		for(size_t i = 0; i != target_length; ++i)
			waveplot->resample[i] = 0.0f;

		memcpy(waveplot->resample, waveplot->values, waveplot->length * sizeof(float));
	}

	float amplitude_factor = (float)(target_amplitude) / WAVEPLOT_RESOLUTION;

	for(size_t i = 0; i != target_length; ++i)
	{
		waveplot->resample[i] = floorf((waveplot->resample[i] * amplitude_factor) + 0.5f);
	}
}

unsigned int generate_sonic_hash(waveplot_t* waveplot)
{
    resample_waveplot(waveplot, 16, (size_t)WAVEPLOT_RESOLUTION);

    float average = 0.0f;
    for(size_t i = 0; i != 16; ++i)
    {
        average += waveplot->resample[i];
    }

    unsigned int result = 0;
    for(size_t i = 0; i != 16; ++i)
    {
        if(waveplot->resample[i] >= average)
            result |= 1 << (15 - i);
    }

    return result;
}
