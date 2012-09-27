AC_INIT([spek],[0.8.1])
AC_CONFIG_SRCDIR([src/spek.cc])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.11.1 foreign no-dist-gzip dist-xz])
AM_SILENT_RULES([yes])

AC_PROG_CC_C99
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_INSTALL
IT_PROG_INTLTOOL([0.40.0])

AC_CANONICAL_HOST
AC_MSG_CHECKING([the OS])
AS_CASE([$host],
	[*-*-mingw*], [
		os="WIN"
		AC_DEFINE([OS_WIN], [1], [Windows])
	],
	[*-*-darwin*], [
		os="OSX"
		AC_DEFINE([OS_OSX], [1], [OS X])
	],
	[*], [
		os="UNIX"
		AC_DEFINE([OS_UNIX], [1], [Unix])
	]
)
AC_MSG_RESULT([$os])

AC_CHECK_LIB(m, log10)

PKG_CHECK_MODULES(FFMPEG, [libavformat >= 52.111 libavcodec >= 52.123 libavutil])

AM_OPTIONS_WXCONFIG
reqwx=2.8.0
AM_PATH_WXCONFIG($reqwx, wx=1)
if test "$wx" != 1; then
	AC_MSG_ERROR([
		wxWidgets must be installed on your system.

		Please check that wx-config is in path, the directory
		where wxWidgets libraries are installed (returned by
		'wx-config --libs' or 'wx-config --static --libs' command)
		is in LD_LIBRARY_PATH or equivalent variable and
		wxWidgets version is $reqwx or above.
		])
fi

GETTEXT_PACKAGE=spek
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Gettext Package])
AC_SUBST(GETTEXT_PACKAGE)
AM_GNU_GETTEXT_VERSION([0.18.1])
AM_PO_SUBDIRS

AC_CONFIG_FILES([
	Makefile
	data/spek.desktop.in
	data/Makefile
	data/icons/Makefile
	data/icons/16x16/Makefile
	data/icons/22x22/Makefile
	data/icons/24x24/Makefile
	data/icons/32x32/Makefile
	data/icons/48x48/Makefile
	data/icons/scalable/Makefile
	dist/osx/Info.plist
	dist/win/spek.wxs
	lib/Makefile
	man/Makefile
	man/spek.1
	po/Makefile.in
	src/Makefile
	tests/Makefile
	web/version
])
AC_OUTPUT
