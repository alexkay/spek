SUBDIRS = \
	data \
	lib \
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
	mandoc -man -Thtml -Ostyle=style.css man/spek.1 > web/man-@VERSION@.html
	sed -e 's#<i>\(http://[^<]*\)</i>#<i><a href="\1">\1</a></i>#g' -i '' web/man-@VERSION@.html

upload:
	rsync -avhz web/nginx.conf server:spek
	rsync -avhz --delete --exclude="*.in" --exclude="nginx.conf" web/ server:spek/static
