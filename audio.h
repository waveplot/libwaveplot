#include <string.h>

typedef struct file_t_t file_t;

typedef struct info_t_t info_t;

typedef struct audio_samples_t_t
{
    float** samples;
    size_t length;
} audio_samples_t;

audio_samples_t* alloc_audio_samples(void);
void free_audio_samples(audio_samples_t* file);
audio_samples_t* get_samples(file_t* file, info_t* info, audio_samples_t* samples);
