# configure.ac

AC_INIT([spek], [0.4])
AC_CONFIG_SRCDIR([src/spek.vala])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([no-dist-gzip dist-bzip2])

# Enable silent rules is available
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AM_MAINTAINER_MODE

AC_PROG_CC
AC_PROG_CC_STDC
AM_PROG_VALAC([0.7.0])
AC_PROG_INSTALL
AC_PROG_INTLTOOL([0.35])

pkg_modules="gtk+-2.0 >= 2.14.0 gstreamer-0.10 >= 0.10.17"
SPEK_PACKAGES="--pkg gtk+-2.0 --pkg gstreamer-0.10"
PKG_CHECK_MODULES(SPEK, [$pkg_modules])
AC_SUBST(SPEK_CFLAGS)
AC_SUBST(SPEK_LIBS)
AC_SUBST(SPEK_PACKAGES)

AC_CHECK_LIB(m, log10f)

# Check for GDK Quartz and MacOSX integration package
_gdk_tgt=`$PKG_CONFIG --variable=target gdk-2.0`
AM_CONDITIONAL([GDK_TARGET_QUARTZ], [test x$_gdk_tgt = xquartz])
if test "x$_gdk_tgt" = xquartz; then
   PKG_CHECK_MODULES(IGE_MAC, ige-mac-integration)
   AC_SUBST(IGE_MAC_LIBS)
   AC_SUBST(IGE_MAC_CFLAGS)
fi

GETTEXT_PACKAGE=spek
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Gettext Package])
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT

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
	po/Makefile.in
	src/Makefile
	vapi/Makefile
])
AC_OUTPUT