SUBDIRS = \
	data \
	man \
	po \
	src \
	tests

EXTRA_DIST = \
	CREDITS.md \
	INSTALL.md \
	LICENSE \
	MANUAL.md \
	README.md \
	lic/Expat \
	lic/GPL \
	lic/IJG \
	lic/LGPL \
	lic/libpng \
	lic/libtiff \
	lic/regex \
	lic/wxWindows \
	lic/zlib \
	tests/samples

DISTCLEANFILES = \
	ABOUT-NLS \
	aclocal.m4 \
	ar-lib \
	compile \
	config.guess \
	config.h.in \
	config.rpath \
	config.sub \
	configure \
	data/Makefile.in \
	data/icons/16x16/Makefile.in \
	data/icons/22x22/Makefile.in \
	data/icons/24x24/Makefile.in \
	data/icons/32x32/Makefile.in \
	data/icons/48x48/Makefile.in \
	data/icons/Makefile.in \
	data/icons/scalable/Makefile.in \
	depcomp \
	install-sh \
	ltmain.sh \
	m4/gettext.m4 \
	m4/host-cpu-c-abi.m4 \
	m4/iconv.m4 \
	m4/intlmacosx.m4 \
	m4/lib-ld.m4 \
	m4/lib-link.m4 \
	m4/lib-prefix.m4 \
	m4/nls.m4 \
	m4/po.m4 \
	m4/progtest.m4 \
	Makefile.in \
	po/*.gmo \
	man/Makefile.in \
	missing \
	po/Makefile.in.in \
	po/Makevars.template \
	po/Rules-quot \
	po/boldquot.sed \
	po/en@boldquot.header \
	po/en@quot.header \
	po/insert-header.sin \
	po/quot.sed \
	po/remove-potcdate.sin \
	po/stamp-po \
	src/Makefile.in \
	tests/Makefile.in \
	tests/samples/perf.wav

.PHONY: man upload

man:
	pandoc --standalone --to=man --output=man/spek.1 MANUAL.md
	pandoc --standalone --to=html --output=web/man-@VERSION@.html MANUAL.md
	sed -e 's#<I>\(http://[^<]*\)</I>#<I><a href="\1">\1</a></I>#g' -i web/man-@VERSION@.html
	sed -e 's#<head>#<head>\
		<style type="text/css">\
			body {font-size: 11pt;}\
			h1, h2, h3 {font-weight: bold;}\
			h1 {font-size: 13pt;}\
			h2, h3 {font-size: 12pt;}\
		</style>#' -i web/man-@VERSION@.html

upload:
	rsync -avhz web/nginx.conf server:spek
	rsync -avhz --delete --exclude="*.in" --exclude="nginx.conf" web/ server:spek/static
