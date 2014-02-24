#include "dr.h"

#include "audio.h"
#include "info.h"

#include <stdint.h>
#include <stdio.h>
#include <math.h>

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
    }
}

void update_dr(audio_samples_t* samples, info_t* info, dr_t* dr)
{
    size_t samples_per_chunk = info->sample_rate * 3;
    static float current_peak = 0.0;
    static float current_rms = 0.0;
    
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
        if((i < half_length) && ((j == (length-half_length)) || (a[i] < b[j])))
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
/*
void postprocess_dr(info_t* info, dr_t* dr)
{
    double dr_rating = 0.0;
    for(size_t channel = 0; i != info->num_channels; ++i)
    {
        dr->channel_rms[channel][dr->length] = sqrt((2.0 * dr->channel_rms[channel][dr->length]) / dr->processed_samples);

        dr_channel_rms_[i].sort([](double first, double second) { return (first > second); });

        size_t values_to_sample = std::max(size_t(1),size_t(dr_channel_rms_[i].size() / 5));

        dr_channel_rms_[i].resize(values_to_sample);

        double rms_sum = 0.0;
        for (double rms_value : dr_channel_rms_[i])
        {
          rms_sum += rms_value * rms_value;
        }

        rms_sum = sqrt(rms_sum / values_to_sample);

        double second_max_value = -100.0;
        double max_value = -100.0;

        for(double peak : dr_channel_peak_[i])
        {
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

        if(dr_channel_peak_[i].size() < 3)
        {
          dr_rating += 20.0*log10(max_value/rms_sum);
        }
        else
        {
          dr_rating += 20.0*log10(second_max_value/rms_sum);
        }
      }

      dr_rating /= num_channels_;
      fprintf(stderr,"DR Level: %2.5f\n",dr_rating);
      fprintf(stdout,"%2.1f",dr_rating);
}*/
