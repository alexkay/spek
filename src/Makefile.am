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
	-DLOCALEDIR=\""$(localedir)"\" \
	-DPKGDATADIR=\""$(pkgdatadir)"\" \
	-DPKGLIBDIR=\""$(pkglibdir)"\"

spek_CFLAGS = \
	$(SPEK_CFLAGS) \
	@CFLAGS@

spek_CXXFLAGS = \
	@CXXFLAGS@

spek_LDADD = \
	$(SPEK_LIBS) \
	@LIBS@
