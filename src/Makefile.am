## Additional flags to pass to aclocal when it is invoked automatically at
## make time. The ${ACLOCAL_FLAGS} variable is picked up from the environment
## to provide a way for the user to supply additional arguments.
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CFLAGS = -std=c99 -O3 -Wall -Wextra -Wpedantic

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
lib_LTLIBRARIES = libwaveplot.la
libwaveplot_la_SOURCES = audio.c audio.h dllexport.h dr.c dr.h file.c file.h info.c info.h typedefs.h waveplot.c waveplot.h
libwaveplot_la_LDFLAGS = -version-info 0:0:0 -lm -lavutil -lavcodec -lavformat -lavresample

install-exec-hook:
	find $(DESTDIR)$(libdir) -type f -name \*.la -delete
