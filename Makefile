libwaveplot:
	gcc -c --std=c99 -O3 -fPIC -Wall waveplot.c audio.c file.c info.c dr.c
	gcc -shared -Wl,-soname,libwaveplot.so.1.0 -o libwaveplot.so.1.0 file.o info.o audio.o  waveplot.o dr.o -lm -lavutil -lavcodec -lavformat

libwptest: libwaveplot
	gcc --std=c99 -O3 -Wall libwptest.c -o libwptest -lwaveplot
