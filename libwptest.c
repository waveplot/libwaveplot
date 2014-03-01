#include "file.h"
#include "info.h"
#include "audio.h"
#include "waveplot.h"
#include "dr.h"

#include "stdio.h"

int main(int argc, char* argv[])
{
    init();
    file_t* f = alloc_file();
    info_t* i = alloc_info();

    load_file("test.flac",f);
    get_info(f,i);
    
    audio_samples_t* a = alloc_audio_samples();
    waveplot_t* w = alloc_waveplot_data();
    dr_t* d = alloc_dr();
    init_dr(i,d);

    while(get_samples(f,i,a) != NULL)
    {
        update_waveplot(a, i, w);
        update_dr(a, i, d);
    }
    
    normalize_waveplot(w);
    postprocess_dr(i,d);
    
    printf("DR: %f\n",d->rating);
}
