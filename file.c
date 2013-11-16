#include "file.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>

void init(void)
{
    av_register_all();
}

file_t* alloc_file(void)
{
    return (file_t*)malloc(sizeof(file_t));
}

void free_file(file_t* file)
{
    free(file);
}

int load_file(char* path, file_t* file)
{
    av_log_set_level(AV_LOG_ERROR);

    file->path = (char*)malloc(strlen(path)+1);
    strcpy(file->path,path);

    AVFormatContext* format_context = NULL;

    if(avformat_open_input(&format_context, file->path, NULL, NULL) < 0)
        return -1;

    if(avformat_find_stream_info(format_context, NULL) < 0)
        return -1;

    int stream_no = av_find_best_stream(format_context,AVMEDIA_TYPE_AUDIO,-1,-1,NULL,0);

    if(stream_no < 0)
        return -1;

    file->stream = format_context->streams[stream_no];

    file->codec_context = file->stream->codec;

    file->codec = avcodec_find_decoder(file->codec_context->codec_id);

    return 0;
}
