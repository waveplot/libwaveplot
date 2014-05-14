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

	int decoded;
	while((decoded = get_samples(a,f,i)) >= 0)
	{
		if(decoded > 0)
		{
			update_waveplot(w, a, i);
			update_dr(d, a, i);
		}
	}

	if(decoded == -1)
	{
		puts("ERROR occurred!");
	}

	finish_waveplot(w);
	finish_dr(d,i);

	resample_waveplot(w, 50, (int)(21 / 2));

	printf("DR: %8.4f\n",d->rating);

	free_dr(d);
	free_waveplot(w);
	free_audio_samples(a);
	free_info(i);
	free_file(f);
}
