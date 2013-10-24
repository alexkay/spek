TESTS = test

if USE_VALGRIND
TESTS_ENVIRONMENT = valgrind --leak-check=full --quiet --error-exitcode=1
endif

check_PROGRAMS = $(TESTS)

test_SOURCES = \
	test-audio.cc \
	test-fft.cc \
	test-utils.cc \
	test.cc \
	test.h

AM_CPPFLAGS = \
	-include config.h \
	-I$(top_srcdir)/src \
	-DSAMPLES_DIR=\"$(srcdir)/samples\" \
	-pthread

AM_CXXFLAGS = \
	$(AVFORMAT_CFLAGS) \
	$(AVCODEC_CFLAGS) \
	$(AVUTIL_CFLAGS)

LDADD = \
	../src/libspek.a \
	$(AVFORMAT_LIBS) \
	$(AVCODEC_LIBS) \
	$(AVUTIL_LIBS)

AM_LDFLAGS = \
	-pthread
