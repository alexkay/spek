noinst_LIBRARIES = libspek.a

libspek_a_SOURCES = \
	spek-audio.cc \
	spek-audio.h \
	spek-fft.cc \
	spek-fft.h \
	spek-palette.cc \
	spek-palette.h \
	spek-pipeline.cc \
	spek-pipeline.h \
	spek-utils.cc \
	spek-utils.h

libspek_a_CPPFLAGS = \
	-include config.h \
	-pthread

libspek_a_CXXFLAGS = \
	$(FFMPEG_CFLAGS)
