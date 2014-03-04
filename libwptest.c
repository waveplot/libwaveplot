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
    get_info(i,f);
    
    audio_samples_t* a = alloc_audio_samples();
    waveplot_t* w = alloc_waveplot();
    dr_t* d = alloc_dr();
    init_dr(d,i);

    while(get_samples(f,i,a) != NULL)
    {
        update_waveplot(w, a, i);
        update_dr(d, a, i);
    }
    
    finish_waveplot(w);
    finish_dr(d,i);
    
    printf("DR: %f\n",d->rating);
}
