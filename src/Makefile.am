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

bin_PROGRAMS = spek

spek_SOURCES = \
	spek-artwork.cc \
	spek-artwork.h \
	spek-audio-desc.cc \
	spek-audio-desc.h \
	spek-events.cc \
	spek-events.h \
	spek-platform.cc \
	spek-platform.h \
	spek-preferences-dialog.cc \
	spek-preferences-dialog.h \
	spek-preferences.cc \
	spek-preferences.h \
	spek-ruler.cc \
	spek-ruler.h \
	spek-spectrogram.cc \
	spek-spectrogram.h \
	spek-window.cc \
	spek-window.h \
	spek.cc

spek_CPPFLAGS = \
	-include config.h \
	-pthread \
	$(WX_CPPFLAGS)

spek_CXXFLAGS = \
	$(WX_CXXFLAGS_ONLY)

spek_LDADD = \
	libspek.a \
	$(FFMPEG_LIBS) \
	$(WX_LIBS)

spek_LDFLAGS = \
	-pthread
