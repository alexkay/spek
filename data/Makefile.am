# Makefile.am

SUBDIRS = icons

desktopdir = $(datadir)/applications
desktop_in_files = spek.desktop.in
desktop_DATA = spek.desktop

EXTRA_DIST = spek.desktop.in
CLEANFILES = spek.desktop

spek.desktop: spek.desktop.in $(wildcard $(top_srcdir)/po/*po)
	$(AM_V_GEN)$(MSGFMT) --desktop --template $< -d $(top_srcdir)/po -o $@
