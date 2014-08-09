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
From GitHub
-----------
When compiling the source from GitHub, a few additional steps are required.
Perform these before the release tarball steps listed below.

    libtoolize
    aclocal
    autoconf
    automake --add-missing

For these commands to successfully run, you'll need to install the GNU
autotools and libtool. These can normally be found in your distribution's
package repository - look for libtool, autoconf, automake and build-essentials.

From Release Tarball
--------------------
The library uses a fairly standard procedure for building libraries on Unix:

    ./configure
    make
    make install

In order to successfully generate a Makefile, a recent version of libavcodec,
libavformat and libavresample will need to be installed on your machine.
