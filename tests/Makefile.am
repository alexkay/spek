check_PROGRAMS = tests

tests_SOURCES = \
	test.c

tests_CPPFLAGS = \
	-include config.h \
	-I$(top_builddir)/src \
	-pthread

tests_CFLAGS = \
	$(FFMPEG_CFLAGS)

tests_LDADD = \
	$(FFMPEG_LIBS)

tests_LDFLAGS = \
	-pthread
