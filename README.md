libwaveplot
===========

A C library for analysing audio, reporting information and generating
WavePlots.

This library is intended to replace the original WavePlotImager executable,
which was bundled with the WavePlot Python script.

Instead of calling an executable, the Python scanner will in future use ctypes
to access the libwaveplot functions. This will avoid having to deal with
exchanging data across between the Imager executable and Python, and also
allows other C/C++ applications to use libwaveplot functions directly.

Compiling
=========

The following steps are used to compile libwaveplot in a GCC style tool:

    gcc -Wall -fPIC info.c file.c
    gcc -shared -Wl,-soname,libwaveplot.so.1 -o libwaveplot.so.1.0 info.o file.o -lavutil -lavcodec -lavformat
