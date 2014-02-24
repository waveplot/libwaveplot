#include "waveplot.h"

#include "audio.h"
#include "info.h"

#include <stdint.h>
#include <stdio.h>
#include <math.h>

waveplot_t* alloc_waveplot_data(void)
{
    waveplot_t* result = (waveplot_t*)malloc(sizeof(waveplot_t));
    result->length = 0;
    
    /* Assume song likely to be less than four minutes
     * 4 mins * 60 secs = 240 secs, 240 secs * 4 Hz = 960 chunks
     */
    result->_capacity = 1024;
    result->values = (float*)malloc(sizeof(float) * result->_capacity);
    return result;
    
}

void free_waveplot_data(waveplot_t* waveplot_data)
{
    free(waveplot_data->values);
    free(waveplot_data);
}

void update_waveplot(audio_samples_t* samples, info_t* info, waveplot_t* waveplot_data)
{
    static uint32_t current_chunk_samples = 0;
    static float current_chunk = 0.0;
    uint32_t samples_per_chunk = (info->sample_rate / 4);

    // Check whether there are too many samples for the current chunk
    if((samples->length + current_chunk_samples) > samples_per_chunk)
    {
        // New chunk - check whether the container needs a resize
        if(waveplot_data->length == waveplot_data->_capacity)
        {
            // Increase the length of the container by 2, and copy the old data
            waveplot_data->_capacity *= 2;
            float* new_array = (float*)malloc(sizeof(float) * waveplot_data->_capacity);
            memcpy(new_array, waveplot_data->values, sizeof(float) * waveplot_data->length);
            free(waveplot_data->values);
            waveplot_data->values = new_array;
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
            waveplot_data->values[waveplot_data->length] = current_chunk;
            ++waveplot_data->length;

            current_chunk = 0.0;
            current_chunk_samples = 0;
        }

        current_chunk += combined_channels;
        ++current_chunk_samples;
    }
}

void normalize_waveplot(waveplot_t* waveplot_data)
{
    static const float sample_weightings[] = {10.0, 8.0, 5.0, 3.0};
    static const size_t num_weightings = sizeof(sample_weightings)/sizeof(float);
    float* processed = (float*)malloc(sizeof(float) * waveplot_data->_capacity);
    
    for(size_t i = 0; i != waveplot_data->length; ++i)
    {
        processed[i] = 0.0f;
    }
    
    for(size_t i = 0; i != waveplot_data->length; ++i)
    {
        float chunk = waveplot_data->values[i];
        processed[i] += chunk * sample_weightings[0];
        for(size_t j = 1; j != num_weightings; ++j)
        {
            float value = chunk * sample_weightings[j];
            size_t index = i + j;

            if(index < waveplot_data->length) {
                processed[index] += value;
            }
            index = i - j;

            if(index < i) {//Will be greater than center if index has wrapped around to ULONG_MAX.
                processed[index] += value;
            }
        }
    }
    
    float max_chunk = 0.0f;
    for(size_t i = 0; i != waveplot_data->length; ++i)
    {
        max_chunk = fmaxf(processed[i], max_chunk);
    }
   

    for(size_t i = 0; i != waveplot_data->length; ++i)
    {
        processed[i] /= max_chunk;
    }
    
    free(waveplot_data->values);
    waveplot_data->values = processed;
}
