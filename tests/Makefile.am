TESTS = \
	test

check_PROGRAMS = $(TESTS)

noinst_HEADERS = \
	test.h

AM_CPPFLAGS = \
	-include config.h \
	-I$(top_builddir)/src \
	-pthread

AM_CFLAGS = \
	$(FFMPEG_CFLAGS)

AM_LDADD = \
	$(FFMPEG_LIBS)

AM_LDFLAGS = \
	-pthread
