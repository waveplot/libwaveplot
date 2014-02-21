libwaveplot:
	gcc -g -c --std=c99 -fPIC -Wall waveplot.c audio.c file.c info.c
	gcc -shared -Wl,-soname,libwaveplot.so.1.0 -o libwaveplot.so.1.0 file.o info.o audio.o  waveplot.o -lavutil -lavcodec -lavformat

libwptest: libwaveplot
	gcc -g --std=c99 -Wall libwptest.c -o libwptest -lwaveplot
