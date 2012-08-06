AC_INIT([spek],[0.7])
AC_CONFIG_SRCDIR([src/spek.cc])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.11.1 foreign no-dist-gzip dist-xz])
AM_MAINTAINER_MODE([yes])
AM_SILENT_RULES([yes])

AC_PROG_CC_C99
AC_PROG_CXX
AC_PROG_INSTALL
IT_PROG_INTLTOOL([0.40.0])

AC_CANONICAL_HOST
AC_MSG_CHECKING([the OS])
AS_CASE([$host],
	[*-*-mingw*], [
		os="WIN"
		AC_DEFINE([OS_WIN], [1], [Platform])
	],
	[*-*-darwin*], [
		os="OSX"
		AC_DEFINE([OS_OSX], [1], [Platform])
	],
	[*], [
		os="UNIX"
		AC_DEFINE([OS_UNIX], [1], [Platform])
	]
)
AC_MSG_RESULT([$os])

AC_CHECK_LIB(m, log10)

pkg_modules="libavformat >= 52.111 libavcodec >= 52.123 libavutil"
PKG_CHECK_MODULES(SPEK, [$pkg_modules])
AC_SUBST(SPEK_CFLAGS)
AC_SUBST(SPEK_LIBS)

AM_OPTIONS_WXCONFIG
reqwx=2.8.0
AM_PATH_WXCONFIG($reqwx, wxWin=1)
if test "$wxWin" != 1; then
	AC_MSG_ERROR([
		wxWidgets must be installed on your system.

		Please check that wx-config is in path, the directory
		where wxWidgets libraries are installed (returned by
		'wx-config --libs' or 'wx-config --static --libs' command)
		is in LD_LIBRARY_PATH or equivalent variable and
		wxWidgets version is $reqwx or above.
		])
fi

CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"
LIBS="$LIBS $WX_LIBS"

GETTEXT_PACKAGE=spek
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Gettext Package])
AC_SUBST(GETTEXT_PACKAGE)
AM_GNU_GETTEXT_VERSION([1.11])
AM_GLIB_GNU_GETTEXT

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
	dist/osx/spek.modules
	dist/win/spek.wxs
	man/Makefile
	man/spek.1
	po/Makefile.in
	src/Makefile
	web/version
])
AC_OUTPUT
