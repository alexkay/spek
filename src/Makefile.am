bin_PROGRAMS = spek

spek_SOURCES = \
	spek.cc \
	spek-audio.c \
	spek-audio.h \
	spek-audio-desc.cc \
	spek-audio-desc.hh \
	spek-fft.c \
	spek-fft.h \
	spek-pipeline.c \
	spek-pipeline.h \
	spek-platform.cc \
	spek-platform.hh \
	spek-preferences.cc \
	spek-preferences.hh \
	spek-ruler.cc \
	spek-ruler.hh \
	spek-spectrogram.cc \
	spek-spectrogram.hh \
	spek-window.cc \
	spek-window.hh

spek_CPPFLAGS = \
	-include config.h \
	-pthread \
	$(WX_CPPFLAGS)

spek_CFLAGS = \
	$(FFMPEG_CFLAGS) \
	$(WX_CFLAGS_ONLY)

spek_CXXFLAGS = \
	$(WX_CXXFLAGS_ONLY)

spek_LDADD = \
	$(FFMPEG_LIBS) \
	$(WX_LIBS)

spek_LDFLAGS = \
	-pthread
