#include <string.h>
#include <stdlib.h>

typedef struct info_t_t info_t;

typedef struct audio_samples_t_t audio_samples_t;

typedef struct waveplot_t_t
{
    float* values;
    size_t length;
    
    size_t _capacity;
} waveplot_t;


waveplot_t* alloc_waveplot_data(void);
void free_waveplot_data(waveplot_t* waveplot_data);
void update_waveplot(audio_samples_t* samples, info_t* info, waveplot_t* waveplot_data);
void normalize_waveplot(waveplot_t* waveplot_data);
