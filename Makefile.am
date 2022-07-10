SUBDIRS = \
	data \
	man \
	po \
	src \
	tests

EXTRA_DIST = \
	INSTALL.md \
	LICENCE.md \
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
