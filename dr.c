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

#include "dr.h"

#include "info.h"
#include "audio.h"

#include <string.h>
#include <stdint.h>
#include <math.h>

#include <stdio.h>

#define min(a, b) ((a) < (b) ? (a) : (b))

/*
 * Implemented based on:
 * http://www.dynamicrange.de/sites/default/files/Measuring%20DR%20ENv3.pdf
 */

void merge_sort(float* values, size_t length);

dr_t* alloc_dr(void)
{
    dr_t* result = (dr_t*)malloc(sizeof(dr_t));

    if(result == NULL)
        return NULL;

    result->length = 0;
    result->_capacity = 0;
    result->rating = 0.0f;
    result->channel_peak = NULL;
    result->channel_rms = NULL;
    result->_processed_samples = 0;
    result->num_channels = 0;
    return result;
}

void free_dr_data(dr_t* dr)
{
    if(dr->channel_peak != NULL)
    {
        for(size_t i = 0; i != dr->num_channels; ++i)
        {
            free(dr->channel_peak[i]);
        }
        free(dr->channel_peak);
        dr->channel_peak = NULL;
    }

    if(dr->channel_rms != NULL)
    {
        for(size_t i = 0; i != dr->num_channels; ++i)
        {
            free(dr->channel_rms[i]);
        }
        free(dr->channel_rms);
        dr->channel_rms = NULL;
    }
}

void free_dr(dr_t* dr)
{
    free_dr_data(dr);
    free(dr);
}

void init_dr(dr_t* dr, info_t* info)
{
    free_dr_data(dr);

    dr->num_channels = info->num_channels;

    dr->channel_peak = (float**)malloc(sizeof(float*)*dr->num_channels);
    dr->channel_rms = (float**)malloc(sizeof(float*)*dr->num_channels);

    dr->_capacity = 128;
    for(int c = 0; c != info->num_channels; ++c)
    {
        dr->channel_peak[c] = (float*)malloc(sizeof(float)*dr->_capacity);
        dr->channel_rms[c] = (float*)malloc(sizeof(float)*dr->_capacity);
        for(size_t i = 0; i != dr->_capacity; ++i)
        {
            dr->channel_peak[c][i] = 0.0f;
            dr->channel_rms[c][i] = 0.0f;
        }
    }
}

void update_dr(dr_t* dr, audio_samples_t* samples, info_t* info)
{
    size_t samples_per_chunk = info->sample_rate * 3;

    if(dr->num_channels != info->num_channels)
    {
        fprintf(stderr,"ERROR: Number of channels inconsistent!\n");
        return;
    }

    size_t num_new_chunks = (samples->length + dr->_processed_samples) / samples_per_chunk;

    // Check whether there are too many samples for the current chunk
    if(num_new_chunks > 0)
    {
        // New chunk - check whether the container needs a resize
        float relative_new_size = (float)(dr->length + num_new_chunks) / dr->_capacity;

        if(relative_new_size >= 1.0)
        {
            size_t new_buffer_size = dr->_capacity * pow(2, ceil(log(relative_new_size+1.0)/log(2)));

            // Increase the length of the container by 2, and copy the old data
            dr->_capacity = new_buffer_size;
            for(int c = 0; c != info->num_channels; ++c)
            {
                float* new_channel_peak = (float*)malloc(sizeof(float) * dr->_capacity);
                float* new_channel_rms = (float*)malloc(sizeof(float) * dr->_capacity);

                for(size_t i = 0; i != dr->_capacity; ++i)
                {
                    new_channel_peak[i] = 0.0f;
                    new_channel_rms[i] = 0.0f;
                }

                memcpy(new_channel_peak, dr->channel_peak[c], sizeof(float) * dr->length);
                memcpy(new_channel_rms, dr->channel_rms[c], sizeof(float) * dr->length);

                free(dr->channel_peak[c]);
                free(dr->channel_rms[c]);

                dr->channel_peak[c] = new_channel_peak;
                dr->channel_rms[c] = new_channel_rms;
            }
        }
    }

    uint32_t cur_ch_num_samples = dr->_processed_samples;
    uint32_t cur_ch_length = dr->length;
    for(size_t channel = 0; channel != info->num_channels; ++channel)
    {
        cur_ch_num_samples = dr->_processed_samples;
        cur_ch_length = dr->length;

        float* cur_ch_peak = &(dr->channel_peak[channel][cur_ch_length]);
        float* cur_ch_rms = &(dr->channel_rms[channel][cur_ch_length]);
        float* cur_ch_samples = samples->samples[channel];
        for(size_t i = 0; i != samples->length; ++i)
        {
            if(cur_ch_num_samples == samples_per_chunk)
            {
                (*cur_ch_rms) = sqrtf((2.0 * (*cur_ch_rms)) / samples_per_chunk);

                ++cur_ch_length;

                cur_ch_peak = &(dr->channel_peak[channel][cur_ch_length]);
                cur_ch_rms = &(dr->channel_rms[channel][cur_ch_length]);

                cur_ch_num_samples = 0;
            }

            float sample = cur_ch_samples[i];
            (*cur_ch_peak) = fmaxf((*cur_ch_peak), fabs(sample));
            (*cur_ch_rms) += sample * sample;
            ++cur_ch_num_samples;
        }
    }

    dr->_processed_samples = cur_ch_num_samples;
    dr->length = cur_ch_length;
}

int compare_samples(const void *s1, const void *s2)
{
    float rms1 = *(float*)s1;
    float rms2 = *(float*)s2;
    if (rms1 > rms2) return -1;
    else if (rms1 < rms2) return 1;
    return 0;
}

void finish_dr(dr_t* dr, info_t* info)
{
    dr->rating = 0.0f;
    for(size_t channel = 0; channel != info->num_channels; ++channel)
    {
        dr->channel_rms[channel][dr->length] = sqrtf((2.0 * dr->channel_rms[channel][dr->length]) / dr->_processed_samples);

        qsort ((void*)dr->channel_rms[channel], dr->length, sizeof(float), compare_samples);

        size_t values_to_sample = dr->length / 5;
        values_to_sample = (values_to_sample ? values_to_sample : 1);

        float rms_sum = 0.0f;
        for(size_t i = 0; i != values_to_sample; ++i)
        {
            float rms_value = dr->channel_rms[channel][i];

            rms_sum += rms_value * rms_value;
        }

        float rms_total = 0.0f;
        float second_max_value = -100.0;
        float max_value = -100.0;
        for(size_t i = 0; i != dr->length; ++i)
        {
            float rms_value = dr->channel_rms[channel][i];
            float peak = dr->channel_peak[channel][i];
            rms_total += rms_value * rms_value;

            if(peak >= max_value)
            {
                second_max_value = max_value;
                max_value = peak;
            }
            else if(peak > second_max_value)
            {
                second_max_value = peak;
            }
        }

        rms_sum = sqrtf(rms_sum / values_to_sample);
        rms_total = sqrtf(rms_total / dr->length);

        if(values_to_sample < 3)
        {
            dr->rating += 20.0f*log10f(max_value/rms_sum);
        }
        else
        {
            dr->rating += 20.0f*log10f(second_max_value/rms_sum);
        }
      }

      dr->rating /= info->num_channels;
}
