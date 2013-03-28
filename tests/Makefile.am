TESTS = test

if USE_VALGRIND
TESTS_ENVIRONMENT = valgrind --leak-check=full --quiet --error-exitcode=1
endif

check_PROGRAMS = $(TESTS)

test_SOURCES = \
	test-audio.cc \
	test-utils.cc \
	test.cc \
	test.h

AM_CPPFLAGS = \
	-include config.h \
	-I$(top_srcdir)/src \
	-DSAMPLES_DIR=\"$(srcdir)/samples\" \
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
