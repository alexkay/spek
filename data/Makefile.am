# Makefile.am

SUBDIRS = icons

desktopdir = $(datadir)/applications
desktop_in_files = spek.desktop.in
desktop_DATA = spek.desktop

appdatadir = $(datadir)/metainfo
appdata_in_files = spek.metainfo.xml.in
appdata_DATA = spek.metainfo.xml

EXTRA_DIST = spek.desktop.in spek.metainfo.xml.in
CLEANFILES = spek.desktop spek.metainfo.xml

spek.desktop: spek.desktop.in $(wildcard $(top_srcdir)/po/*po)
	$(AM_V_GEN)$(MSGFMT) --desktop --template $< -d $(top_srcdir)/po -o $@

spek.metainfo.xml: spek.metainfo.xml.in $(wildcard $(top_srcdir)/po/*po)
	$(AM_V_GEN)$(MSGFMT) --xml --template $< -d $(top_srcdir)/po -o $@
