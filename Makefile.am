SUBDIRS = \
	data \
	lib \
	man \
	po \
	src \
	tests

EXTRA_DIST = \
	AUTHORS.md \
	INSTALL.md \
	README.md \
	dist/osx/DS_Store \
	dist/osx/Info.plist \
	dist/osx/README.md \
	dist/osx/Spek.icns \
	dist/osx/about.png \
	dist/osx/bundle.sh \
	dist/osx/close.png \
	dist/osx/open.png \
	dist/osx/save.png \
	dist/win/README.md \
	dist/win/SpekInstallDir.wxs \
	dist/win/SpekInstallDirDlg.wxs
	dist/win/about.ico \
	dist/win/banner.bmp \
	dist/win/bundle.bat \
	dist/win/bundle.sh \
	dist/win/close.ico \
	dist/win/dialog.bmp \
	dist/win/fix-msi.js \
	dist/win/open.ico \
	dist/win/save.ico \
	dist/win/spek.ico \
	dist/win/spek.rc \
	dist/win/spek.wxs \
	intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

DISTCLEANFILES = \
	intltool-extract \
	intltool-merge \
	intltool-update

.PHONY: upload

upload:
	rsync -avhz web/nginx.conf server:spek
	rsync -avhz --delete --exclude="*.in" --exclude="nginx.conf" web/ server:spek/static
