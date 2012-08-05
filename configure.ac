AC_INIT([spek],[0.7])
AC_CONFIG_SRCDIR([src/spek.cc])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.11 foreign no-dist-gzip dist-xz])
AM_SILENT_RULES

AM_MAINTAINER_MODE

AC_PROG_CC_C99
AC_PROG_CXX
AC_PROG_INSTALL
IT_PROG_INTLTOOL([0.35])

pkg_modules="libavformat >= 52.111 libavcodec >= 52.123 libavutil"
PKG_CHECK_MODULES(SPEK, [$pkg_modules])
AC_SUBST(SPEK_CFLAGS)
AC_SUBST(SPEK_LIBS)

AC_CHECK_LIB(m, log10)

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
