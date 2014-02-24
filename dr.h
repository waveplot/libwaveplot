#include <string.h>
#include <stdlib.h>

typedef struct info_t_t info_t;

typedef struct audio_samples_t_t audio_samples_t;

typedef struct dr_t_t
{
    float** channel_peak;
    float** channel_rms;
    size_t processed_samples;
    
    size_t length;
    
    size_t _capacity;
} dr_t;

dr_t* alloc_dr(void);
void free_dr(dr_t* dr);
void init_dr(info_t* info, dr_t* dr);
void update_dr(audio_samples_t* samples, info_t* info, dr_t* dr_data);
void merge_sort(float* values, size_t length);
