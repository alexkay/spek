SUBDIRS = \
	data \
	man \
	po \
	src \
	tests

EXTRA_DIST = \
	INSTALL.md \
	LICENCE.md \
	README.md \
	intltool-extract.in \
	intltool-merge.in \
	intltool-update.in \
	lic/Expat \
	lic/GPL \
	lic/IJG \
	lic/LGPL \
	lic/libpng \
	lic/libtiff \
	lic/regex \
	lic/wxWindows \
	lic/zlib

DISTCLEANFILES = \
	intltool-extract \
	intltool-merge \
	intltool-update

.PHONY: man upload

man:
	man man/spek.1 | man2html -title "Spek @VERSION@ manual" > web/man-@VERSION@.html
	sed -e 's#<I>\(http://[^<]*\)</I>#<I><a href="\1">\1</a></I>#g' -i '' web/man-@VERSION@.html
	sed -e 's#<HEAD>#<HEAD>\
		<style type="text/css">\
			body {font-size: 11pt;}\
			h1, h2 {font-weight: bold;}\
			h1 {font-size: 16pt;}\
			h2 {font-size: 12pt;}\
		</style>#' -i '' web/man-@VERSION@.html

upload:
	rsync -avhz web/nginx.conf server:spek
	rsync -avhz --delete --exclude="*.in" --exclude="nginx.conf" web/ server:spek/static
