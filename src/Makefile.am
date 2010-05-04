bin_PROGRAMS = spek

spek_SOURCES = \
	spek.vala \
	spek-source.vala \
	spek-window.vala

INCLUDES = \
	-include config.h \
	$(SPEK_CFLAGS) \
	-DLOCALEDIR=\""$(localedir)"\" \
	-DPKGDATADIR=\""$(pkgdatadir)"\" \
	-DPKGLIBDIR=\""$(pkglibdir)"\"

VALAFLAGS = \
	--vapidir=$(srcdir)/../vapi --pkg config \
	@SPEK_PACKAGES@

spek_LDADD = \
	$(SPEK_LIBS)
