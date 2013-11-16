#include "info.h"
#include "file.h"

#include <string.h>

info_t* alloc_info(void)
{
    return (info_t*)malloc(sizeof(info_t));
}

void free_info(info_t* info)
{
    free(info);
}

void get_info(file_t* file, info_t* info)
{
    /* Calculate the approximate duration of the stream - assumed to be less
     * than 18 hours long. */
    info->duration_secs = (uint32_t)((file->stream->duration * file->stream->time_base.num) / file->stream->time_base.den);

    info->num_channels = (uint8_t)file->codec_context->channels;
    info->sample_rate  = (uint32_t)file->codec_context->sample_rate;
    info->bit_rate = (uint32_t)file->codec_context->bit_rate;

    info->file_format = (char*)malloc(strlen(file->codec->name)+1);
    strcpy(info->file_format,file->codec->name);
}
