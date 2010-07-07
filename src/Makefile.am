bin_PROGRAMS = spek

spek_SOURCES = \
	spek.vala \
	spek-audio.c \
	spek-fft.c \
	spek-message-bar.vala \
	spek-pipeline.vala \
	spek-ruler.vala \
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
	--thread \
	--vapidir=$(srcdir)/../vapi \
	--pkg config \
	--pkg spek-audio \
	--pkg spek-fft \
	@SPEK_PACKAGES@

spek_LDADD = \
	$(SPEK_LIBS) \
	$(IGE_MAC_LIBS)

EXTRA_DIST = \
	spek-audio.h \
	spek-fft.h
