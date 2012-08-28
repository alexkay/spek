noinst_LIBRARIES = libspek.a

libspek_a_SOURCES = \
	spek-audio.c \
	spek-audio.h \
	spek-fft.c \
	spek-fft.h \
	spek-palette.c \
	spek-palette.h \
	spek-pipeline.c \
	spek-pipeline.h \
	spek-utils.c \
	spek-utils.h

libspek_a_CPPFLAGS = \
	-include config.h \
	-pthread

libspek_a_CFLAGS = \
	$(FFMPEG_CFLAGS)
