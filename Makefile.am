SUBDIRS = \
	data \
	lib \
	man \
	po \
	src \
	tests

EXTRA_DIST = \
	intltool-extract.in \
	intltool-merge.in \
	intltool-update.in \
	dist/osx/DS_Store \
	dist/osx/Info.plist \
	dist/osx/README \
	dist/osx/Spek.icns \
	dist/osx/bundle.sh \
	dist/osx/gtkrc \
	dist/osx/launcher.sh \
	dist/osx/spek.bundle \
	dist/osx/spek.modules \
	dist/win/banner.bmp \
	dist/win/bundle.sh \
	dist/win/dialog.bmp \
	dist/win/fix-msi.js \
	dist/win/spek.ico \
	dist/win/spek.rc \
	dist/win/spek.wxs

DISTCLEANFILES = \
	intltool-extract \
	intltool-merge \
	intltool-update

.PHONY: upload

upload:
	rsync -avhz web/nginx.conf server:spek
	rsync -avhz --delete --exclude="*.in" --exclude="nginx.conf" web/ server:spek/static
