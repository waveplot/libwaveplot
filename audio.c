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

#include "audio.h"

#include "info.h"
#include "file.h"

#include "libavcodec/avcodec.h"
#include "libavutil/samplefmt.h"

#include <stdlib.h>
#include <stdint.h>

audio_samples_t* alloc_audio_samples(void)
{
    audio_samples_t* result = (audio_samples_t*)malloc(sizeof(audio_samples_t));
    result->samples = NULL;
    return result;
    
}

void free_audio_samples(audio_samples_t* samples)
{
    free(samples);
}

float** normalize_int16_t_samples(uint8_t** samples, info_t* info, size_t num_samples)
{
    int16_t** converted_input = (int16_t**)samples;
    float** normalized_data = (float**)malloc(info->num_channels * sizeof(float*));

    for(size_t channel = 0; channel != info->num_channels; ++channel)
    {
        normalized_data[channel] = (float*)malloc(num_samples * sizeof(float));
        for(size_t i = 0; i != num_samples; ++i)
        {
            // Add on 0.5 here because singed integer types aren't +/- balanced
            normalized_data[channel][i] = ((float)(converted_input[channel][i]) + 0.5f) / 32767.5f;
        }
    }

    return normalized_data;
}

float** normalize_int32_t_samples(uint8_t** samples, info_t* info, size_t num_samples)
{
    int32_t** converted_input = (int32_t**)samples;
    float** normalized_data = (float**)malloc(info->num_channels * sizeof(float*));

    for(size_t channel = 0; channel != info->num_channels; ++channel)
    {
        normalized_data[channel] = (float*)malloc(num_samples * sizeof(float));
        for(size_t i = 0; i != num_samples; ++i)
        {
            normalized_data[channel][i] = ((float)(converted_input[channel][i]) + 0.5f) / 2147483647.5f;
        }
    }

    return normalized_data;
}

float** normalize_float_samples(uint8_t** samples, info_t* info, size_t num_samples)
{
    float** normalized_data = (float**)malloc(info->num_channels * sizeof(float*));

    for(size_t channel = 0; channel != info->num_channels; ++channel)
    {
        normalized_data[channel] = (float*)malloc(num_samples * sizeof(float));

        memcpy(normalized_data[channel], samples[channel], num_samples*sizeof(float));
    }

    return normalized_data;
}

float** normalize_double_samples(uint8_t** samples, info_t* info, size_t num_samples)
{
    double** converted_input = (double**)samples;
    float** normalized_data = (float**)malloc(info->num_channels * sizeof(float*));

    for(size_t channel = 0; channel != info->num_channels; ++channel)
    {
        normalized_data[channel] = (float*)malloc(num_samples * sizeof(float));

        for(size_t i = 0; i != num_samples; ++i)
        {
            normalized_data[channel][i] = (float)(converted_input[channel][i]);
        }
    }

    return normalized_data;
}

uint8_t** split_interleaved_data(uint8_t** data, info_t* info, size_t total_bytes, uint8_t bytes_per_sample)
{
    uint8_t num_channels = info->num_channels;
    uint8_t** channel_samples = (uint8_t**)malloc(sizeof(uint8_t*)*num_channels);
    size_t bytes_per_channel = total_bytes / num_channels;

    for(size_t channel = 0; channel != num_channels; ++channel)
    {
        channel_samples[channel] = (uint8_t*)malloc(sizeof(uint8_t)*bytes_per_channel);
        uint8_t* initial_offset = data[0] + (channel * bytes_per_sample);

        for(size_t i = 0; i != bytes_per_channel; i += bytes_per_sample)
        {
            uint8_t* src = initial_offset + (i * num_channels);
            uint8_t* dst = channel_samples[channel] + i;
            memcpy(dst, src, bytes_per_sample);
        }
    }

    return channel_samples;
}

uint8_t** copy_planar_data(uint8_t** data, info_t* info, size_t total_bytes, uint8_t bytes_per_sample)
{
    uint8_t num_channels = info->num_channels;
    uint8_t** channel_samples = (uint8_t**)malloc(sizeof(uint8_t*)*info->num_channels);
    size_t bytes_per_channel = total_bytes / num_channels;
    for(size_t channel = 0; channel != num_channels; ++channel)
    {
        channel_samples[channel] = (uint8_t*)malloc(sizeof(uint8_t)*bytes_per_channel);
        memcpy(channel_samples[channel], data[channel], bytes_per_channel);
    }

    return channel_samples;
}

audio_samples_t* get_samples(audio_samples_t* samples, file_t* file, info_t* info)
{
    AVCodec* codec = avcodec_find_decoder(file->codec_context->codec_id);

    if (avcodec_open2(file->codec_context, codec, NULL) < 0)
        return NULL;

    AVPacket packet;
    int decoded;

    enum AVSampleFormat sample_format = file->codec_context->sample_fmt;
    AVFrame* frame = avcodec_alloc_frame();

    if(av_read_frame(file->format_context, &packet) != 0)
        return NULL;

    avcodec_decode_audio4(file->codec_context, frame, &decoded, &packet);

    if(decoded != 0)
    {
        if(frame->format != sample_format)
            return NULL;
    }
    else
    {
        return NULL;
    }

    int total_bytes = av_samples_get_buffer_size(NULL, info->num_channels, frame->nb_samples, sample_format, 1);
    uint8_t bytes_per_sample = (uint8_t)av_get_bytes_per_sample(sample_format);

    samples->length = total_bytes / (info->num_channels * bytes_per_sample);

    uint8_t** data_by_channel = NULL;
    //Split interleaved data into separate buffers for each channel.
    if((sample_format == AV_SAMPLE_FMT_S16) ||
       (sample_format == AV_SAMPLE_FMT_S32) ||
       (sample_format == AV_SAMPLE_FMT_FLT) ||
       (sample_format == AV_SAMPLE_FMT_DBL))
    {
        data_by_channel = split_interleaved_data(frame->data, info, total_bytes, bytes_per_sample);
    }
    else if((sample_format == AV_SAMPLE_FMT_S16P) ||
            (sample_format == AV_SAMPLE_FMT_S32P) ||
            (sample_format == AV_SAMPLE_FMT_FLTP) ||
            (sample_format == AV_SAMPLE_FMT_DBLP))
    {
        data_by_channel = copy_planar_data(frame->data, info, total_bytes, bytes_per_sample);
    }
    else
    {
        return NULL;
    }

    //free the frame and its data here - we've got a copy in planar form
    free(samples->samples); //free samples if they've been allocated before.

    //This bit converts samples from whatever format they were originally in, to a float between 0.0 and 1.0.
    switch(sample_format)
    {
      case AV_SAMPLE_FMT_S16:
      case AV_SAMPLE_FMT_S16P:
        samples->samples = normalize_int16_t_samples(data_by_channel, info, samples->length);
        break;
      case AV_SAMPLE_FMT_S32:
      case AV_SAMPLE_FMT_S32P:
        samples->samples = normalize_int32_t_samples(data_by_channel, info, samples->length);
        break;
      case AV_SAMPLE_FMT_FLT:
      case AV_SAMPLE_FMT_FLTP:
        samples->samples = normalize_float_samples(data_by_channel, info, samples->length);
        break;
      case AV_SAMPLE_FMT_DBL:
      case AV_SAMPLE_FMT_DBLP:
        samples->samples = normalize_double_samples(data_by_channel, info, samples->length);
        break;
      default:
        //Shouldn't ever get here.
        break;
    }

    //free planar data allocated earlier, since we have it normalized now.
    for(size_t channel = 0; channel != info->num_channels; ++channel)
    {
        free(data_by_channel[channel]);
    }
    free(data_by_channel);

#if (LIBAVCODEC_VERSION_MAJOR > 53)
    avcodec_free_frame(&frame);
#else
    av_free(frame);
#endif

    return samples;
}
