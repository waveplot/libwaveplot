#include "file.h"
#include "info.h"
#include "audio.h"
#include "waveplot.h"

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

    while(get_samples(f,i,a) != 0)
    {
        update_waveplot(a, i, w);
    }
    
    normalize_waveplot(w);
    
    for(size_t i = 0; i != w->length; ++i)
    {
        printf("%f,%f\n",w->values[i],-w->values[i]);
    }
}
