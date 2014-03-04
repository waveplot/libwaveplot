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

The included Makefile can be used to compile libwaveplot or libwptest (testing
application):

    make libwaveplot
    make libwptest

In order to successfully build these targets, a recent version of libav will
need to be installed on your machine.
