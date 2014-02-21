
#include <stdint.h>
#include <stdlib.h>

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

typedef struct info_t_t info_t;

typedef struct file_t_t
{
    char* path;

    info_t* info;

    //waveplot_t* waveplot;

    AVStream* stream;
    AVCodecContext* codec_context;
    AVCodec* codec;
    AVFormatContext* format_context;

} file_t;

/* Initialize WavePlot and libav */
void init(void);

/* Allocate storage for a file structure, and deallocate. */
file_t* alloc_file(void);
void free_file(file_t*);

/* Load a file with libav, into an allocated file structure. */
int load_file(char*, file_t*);
