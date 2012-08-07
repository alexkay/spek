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
	$(SPEK_CFLAGS) \
	-DLOCALEDIR=\""$(localedir)"\" \
	-DPKGDATADIR=\""$(pkgdatadir)"\" \
	-DPKGLIBDIR=\""$(pkglibdir)"\"

spek_CFLAGS = \
	@CFLAGS@

spek_CXXFLAGS = \
	@CXXFLAGS@

spek_LDADD = \
	@LIBS@ \
	$(SPEK_LIBS)
