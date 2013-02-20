TESTS = test

check_PROGRAMS = $(TESTS)

test_SOURCES = \
	test-utils.cc \
	test.cc \
	test.h

AM_CPPFLAGS = \
	-include config.h \
	-I$(top_srcdir)/src \
	-pthread \
	$(WX_CPPFLAGS)

AM_CXXFLAGS = \
	$(FFMPEG_CFLAGS) \
	$(WX_CXXFLAGS_ONLY)

LDADD = \
	../src/libspek.a \
	$(FFMPEG_LIBS) \
	$(WX_LIBS)

AM_LDFLAGS = \
	-pthread
