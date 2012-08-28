TESTS = \
	test-utils \
	test

check_PROGRAMS = $(TESTS)

noinst_HEADERS = \
	test.h

AM_CPPFLAGS = \
	-include config.h \
	-I$(top_builddir)/lib \
	-pthread

AM_CFLAGS = \
	$(FFMPEG_CFLAGS)

LDADD = \
	../lib/libspek.a \
	$(FFMPEG_LIBS)

AM_LDFLAGS = \
	-pthread
