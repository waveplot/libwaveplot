#include "dr.h"

#include "audio.h"
#include "info.h"

#include <stdint.h>
#include <stdio.h>
#include <math.h>

/*
 * Implemented based on:
 * http://www.dynamicrange.de/sites/default/files/Measuring%20DR%20ENv3.pdf
 */

dr_t* alloc_dr(void)
{
    dr_t* result = (dr_t*)malloc(sizeof(dr_t));
    result->length = 0;
    result->_capacity = 0;
    result->channel_peak = NULL;
    result->channel_rms = NULL;
    result->processed_samples = 0;
    return result;
}

void free_dr(dr_t* dr)
{
    free(dr->channel_peak);
    free(dr->channel_rms);
    free(dr);
}

void init_dr(info_t* info, dr_t* dr)
{
    dr->channel_peak = (float**)malloc(sizeof(float*)*info->num_channels);
    dr->channel_rms = (float**)malloc(sizeof(float*)*info->num_channels);
    
    dr->_capacity = 128;
    for(int i = 0; i != info->num_channels; ++i)
    {
        dr->channel_peak[i] = (float*)malloc(sizeof(float)*dr->_capacity);
        dr->channel_rms[i] = (float*)malloc(sizeof(float)*dr->_capacity);
        dr->channel_peak[i][0] = 0.0f;
        dr->channel_rms[i][0] = 0.0f;
    }
}

void update_dr(audio_samples_t* samples, info_t* info, dr_t* dr)
{
    size_t samples_per_chunk = info->sample_rate * 3;
    
    // Check whether there are too many samples for the current chunk
    if((samples->length + dr->processed_samples) > samples_per_chunk)
    {
        // New chunk - check whether the container needs a resize
        if(dr->length == dr->_capacity)
        {
            // Increase the length of the container by 2, and copy the old data
            dr->_capacity *= 2;
            for(int i = 0; i != info->num_channels; ++i)
            {
                float* new_array = (float*)malloc(sizeof(float) * dr->_capacity);
                memcpy(new_array, dr->channel_rms[i], sizeof(float) * dr->length);
                free(dr->channel_rms[i]);
                dr->channel_rms[i] = new_array;
                
                new_array = (float*)malloc(sizeof(float) * dr->_capacity);
                memcpy(new_array, dr->channel_peak[i], sizeof(float) * dr->length);
                free(dr->channel_peak[i]);
                dr->channel_peak[i] = new_array;
            }
        }
    }
    
    uint32_t cur_ch_num_samples = dr->processed_samples;
    uint32_t cur_ch_length = dr->length;
    for(size_t channel = 0; channel != info->num_channels; ++channel)
    {
        cur_ch_num_samples = dr->processed_samples;
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
                (*cur_ch_peak) = 0.0f;
                (*cur_ch_rms) = 0.0f;

                cur_ch_num_samples = 0;
            }
      
            float sample = cur_ch_samples[i];
            (*cur_ch_peak) = fmaxf((*cur_ch_peak), fabs(sample));
            (*cur_ch_rms) += sample * sample;
            ++cur_ch_num_samples;
        }
    }
    
    dr->processed_samples = cur_ch_num_samples;
    dr->length = cur_ch_length;   
}

void merge_sort(float* values, size_t length)
{
    size_t half_length = length/2;
    float* a = values;
    float* b = values + half_length;
    
    if(length > 2)
    {
        merge_sort(a, half_length);
        merge_sort(b, length - half_length);
    }
    
    if(length == 1)
        return;
    
    float* temp = (float*)malloc(sizeof(float)*length);
    
    for(size_t i = 0, j = 0, k = 0; k != length; ++k)
    {
        if((i < half_length) && ((j == (length-half_length)) || (a[i] > b[j])))
        {
            temp[k] = a[i];
            i++;
        }
        else
        {
            temp[k] = b[j];
            j++;
        }
    }
    
    memcpy(values, temp, length*sizeof(float));    
    free(temp);
}

void postprocess_dr(info_t* info, dr_t* dr)
{
    dr->rating = 0.0f;
    for(size_t channel = 0; channel != info->num_channels; ++channel)
    {
        dr->channel_rms[channel][dr->length] = sqrtf((2.0 * dr->channel_rms[channel][dr->length]) / dr->processed_samples);

        merge_sort(dr->channel_rms[channel], dr->length);

        size_t values_to_sample = dr->length / 5;
        values_to_sample = (values_to_sample ? values_to_sample : 1);

        
        float rms_sum = 0.0f;
        float second_max_value = -100.0;
        float max_value = -100.0;
        for(size_t i = 0; i != values_to_sample; ++i)
        {
            float rms_value = dr->channel_rms[channel][i];
            float peak = dr->channel_peak[channel][i];
            
            rms_sum += rms_value * rms_value;
            
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
