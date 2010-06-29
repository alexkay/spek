bin_PROGRAMS = spek

spek_SOURCES = \
	spek.vala \
	spek-audio.c \
	spek-pipeline.vala \
	spek-ruler.vala \
	spek-source.vala \
	spek-spectrogram.vala \
	spek-window.vala

INCLUDES = \
	-include config.h \
	$(SPEK_CFLAGS) \
	$(IGE_MAC_CFLAGS) \
	-DLOCALEDIR=\""$(localedir)"\" \
	-DPKGDATADIR=\""$(pkgdatadir)"\" \
	-DPKGLIBDIR=\""$(pkglibdir)"\"

VALAFLAGS = \
	--vapidir=$(srcdir)/../vapi \
	--pkg config \
	--pkg spek-audio \
	@SPEK_PACKAGES@

spek_LDADD = \
	$(SPEK_LIBS) \
	$(IGE_MAC_LIBS)
