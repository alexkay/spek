bin_PROGRAMS = spek

spek_SOURCES = \
	spek-artwork.hh \
	spek-artwork.cc \
	spek-audio-desc.cc \
	spek-audio-desc.hh \
	spek-events.cc \
	spek-events.hh \
	spek-platform.cc \
	spek-platform.hh \
	spek-preferences-dialog.cc \
	spek-preferences-dialog.hh \
	spek-preferences.cc \
	spek-preferences.hh \
	spek-ruler.cc \
	spek-ruler.hh \
	spek-spectrogram.cc \
	spek-spectrogram.hh \
	spek-window.cc \
	spek-window.hh \
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
