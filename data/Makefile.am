# Makefile.am

SUBDIRS = icons

desktopdir = $(datadir)/applications
desktop_in_files = spek.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = \
	$(desktop_in_files)

CLEANFILES = \
	$(desktop_DATA)
