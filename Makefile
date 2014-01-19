libwaveplot:
	gcc -c --std=c99 -fPIC -Wall audio.c file.c info.c
	gcc -shared -Wl,-soname,libwaveplot.so.1.0 -o libwaveplot.so.1.0 file.o info.o audio.o -lavutil -lavcodec -lavformat
