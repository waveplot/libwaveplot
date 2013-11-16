
#include <stdint.h>

typedef struct file_t_t file_t;

typedef struct info_t_t
{
    uint32_t duration_secs;

    uint8_t num_channels;

    uint16_t bit_depth;
    uint32_t bit_rate;
    uint32_t sample_rate;

    char* file_format;
} info_t;

/* For allocating and freeing information structures */
info_t* alloc_info(void);
void free_info(info_t*);

void get_info(file_t*, info_t*);
