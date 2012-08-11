bin_PROGRAMS = spek

spek_SOURCES = \
	spek.cc \
	spek-audio.c \
	spek-audio.h \
	spek-fft.c \
	spek-fft.h \
	spek-platform.cc \
	spek-platform.hh \
	spek-preferences.cc \
	spek-preferences.hh \
	spek-window.cc \
	spek-window.hh

spek_CPPFLAGS = \
	-include config.h \
	$(WX_CPPFLAGS)

spek_CFLAGS = \
	$(FFMPEG_CFLAGS) \
	$(WX_CFLAGS_ONLY)

spek_CXXFLAGS = \
	$(FFMPEG_CFLAGS) \
	$(WX_CXXFLAGS_ONLY)

spek_LDADD = \
	$(FFMPEG_LIBS) \
	$(WX_LIBS)