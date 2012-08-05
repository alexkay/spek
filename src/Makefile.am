bin_PROGRAMS = spek

spek_SOURCES = \
	spek.cc

AM_CPPFLAGS = \
	-include config.h \
	$(SPEK_CFLAGS) \
	-DLOCALEDIR=\""$(localedir)"\" \
	-DPKGDATADIR=\""$(pkgdatadir)"\" \
	-DPKGLIBDIR=\""$(pkglibdir)"\"

spek_LDADD = \
	$(SPEK_LIBS)
