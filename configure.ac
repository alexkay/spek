AC_INIT([spek],[0.8.2])
AC_CONFIG_SRCDIR([src/spek.cc])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.11.1 foreign no-dist-gzip dist-xz serial-tests])
AM_SILENT_RULES([yes])

AC_LANG([C++])
AC_PROG_CXX
CXXFLAGS="$CXXFLAGS -std=gnu++11 -Wall -Wextra"
AC_PROG_CXXCPP
AC_PROG_LIBTOOL
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

AC_CHECK_PROG(HAVE_VALGRIND, valgrind, yes, no)
AC_ARG_ENABLE(
    [valgrind],
    AS_HELP_STRING([--enable-valgrind], [Run tests under valgrind]),
    [use_valgrind=$enableval],
    [use_valgrind=auto]
)
AS_IF(
    [test "x$use_valgrind" = xyes -a "x$HAVE_VALGRIND" = xno], [AC_MSG_ERROR([Valgrind not found])],
    [AM_CONDITIONAL([USE_VALGRIND], [test "x$use_valgrind" != xno -a x$HAVE_VALGRIND = xyes])]
)
AM_COND_IF([USE_VALGRIND], [use_valgrind=yes], [use_valgrind=no])

AC_CHECK_LIB(m, log10)

PKG_CHECK_MODULES(AVFORMAT, [libavformat >= 53.17])
PKG_CHECK_MODULES(AVCODEC, [libavcodec >= 53.25])
PKG_CHECK_MODULES(AVUTIL, [libavutil >= 51.17])

AM_OPTIONS_WXCONFIG
reqwx=3.0.0
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
	man/Makefile
	po/Makefile.in
	src/Makefile
	tests/Makefile
	web/version
])
AC_OUTPUT

cat <<EOF

${PACKAGE}-${VERSION}

    Install Prefix: ${prefix}
    C++ Compiler:   ${CXX}
    OS:             ${os}
    wxWidgets:      ${WX_VERSION}
    Use Valgrind:   ${use_valgrind}

EOF
