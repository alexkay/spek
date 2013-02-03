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
	-I$(top_srcdir)/lib \
	$(WX_CPPFLAGS)

spek_CXXFLAGS = \
	$(WX_CXXFLAGS_ONLY)

spek_LDADD = \
	../lib/libspek.a \
	$(FFMPEG_LIBS) \
	$(WX_LIBS)

spek_LDFLAGS = \
	-pthread
