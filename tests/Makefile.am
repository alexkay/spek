TESTS = test

check_PROGRAMS = $(TESTS)

test_SOURCES = \
	test-utils.cc \
	test.cc \
	test.h

AM_CPPFLAGS = \
	-include config.h \
	-I$(top_srcdir)/src \
	-pthread

AM_CXXFLAGS = \
	$(FFMPEG_CFLAGS)

LDADD = \
	../src/libspek.a \
	$(FFMPEG_LIBS)

AM_LDFLAGS = \
	-pthread
