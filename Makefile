NAME    = spek
BIN     = ${NAME}${EXT}
LIB     = lib${NAME}.a
VERSION = 0.8.5

PREFIX = /usr/local

LIBS       = -lavcodec -lavformat -lavutil `wx-config --libs`

TESTS_LIBS = -lavcodec -lavformat -lavutil

TESTS_CPPFLAGS = -DSAMPLES_DIR=\"samples\" -Isrc -pthread ${CPPFLAGS}
TESTS_CXXFLAGS = -Os ${TESTS_CPPFLAGS} ${CXXFLAGS}
TESTS_LDFLAGS  = -pthread ${TESTS_LIBS} ${LDFLAGS}

SPEK_CPPFLAGS = ${OS_FLAGS} -DGETTEXT_PACKAGE=\"'${NAME}'\" -DPACKAGE_NAME=\"'${NAME}'\" -DPACKAGE_VERSION=\"'${VERSION}'\" `wx-config --cppflags` ${CPPFLAGS}
SPEK_CXXFLAGS = -Os `wx-config --cxxflags` ${SPEK_CPPFLAGS} ${CXXFLAGS}
SPEK_LDFLAGS  = ${LIBS} ${LDFLAGS}

CXX = g++

GMO = \
	po/bs.gmo \
	po/ca.gmo \
	po/cs.gmo \
	po/da.gmo \
	po/de.gmo \
	po/el.gmo \
	po/eo.gmo \
	po/es.gmo \
	po/fi.gmo \
	po/fr.gmo \
	po/gl.gmo \
	po/he.gmo \
	po/hr.gmo \
	po/hu.gmo \
	po/id.gmo \
	po/it.gmo \
	po/ja.gmo \
	po/ko.gmo \
	po/lv.gmo \
	po/nb.gmo \
	po/nl.gmo \
	po/nn.gmo \
	po/pl.gmo \
	po/pt_BR.gmo \
	po/ru.gmo \
	po/sk.gmo \
	po/sr@latin.gmo \
	po/sv.gmo \
	po/th.gmo \
	po/tr.gmo \
	po/uk.gmo \
	po/vi.gmo \
	po/zh_CN.gmo \
	po/zh_TW.gmo \

OBJ_LIB = \
	src/spek-audio.o \
	src/spek-fft.o \
	src/spek-palette.o \
	src/spek-pipeline.o \
	src/spek-utils.o \

OBJ_PERF = \
	tests/perf.o \

OBJ_TEST = \
	tests/test-audio.o \
	tests/test-fft.o \
	tests/test-utils.o \
	tests/test.o \

OBJ = \
	src/spek-artwork.o \
	src/spek-events.o \
	src/spek.o \
	src/spek-platform.o \
	src/spek-preferences-dialog.o \
	src/spek-preferences.o \
	src/spek-ruler.o \
	src/spek-spectrogram.o \
	src/spek-window.o \

all:
	@echo "To build ${NAME} type make and one the following: osx, unix or win."
	@echo "E.g. \`make unix\`"

options:
	@echo "${NAME} build options:"
	@echo "CXXFLAGS = ${SPEK_CXXFLAGS}"
	@echo "LDFLAGS  = ${SPEK_LDFLAGS}"
	@echo "CXX      = ${CXX}"

.cc.o:
	${CXX} -c ${SPEK_CXXFLAGS} $< -o $@

.po.gmo:
	msgfmt -o $@ $<

${LIB}: ${OBJ_LIB}
	ar crs $@ ${OBJ_LIB}

${BIN}: ${LIB} ${OBJ}
	${CXX} -o ${BIN} ${OBJ} ${LIB} ${SPEK_LDFLAGS}

data/${NAME}.desktop:
	sed '/^#/d' ${@}.in > ${@}.in.tmp
	msgfmt -d po -o $@ --desktop --template=${@}.in.tmp
	rm ${@}.in.tmp

data/${NAME}.metainfo.xml:
	msgfmt -d po -o $@ --xml --template=${@}.in

osx:
	@make options ${BIN} OS_FLAGS=-DOS_OSX

unix:
	@make options ${BIN} OS_FLAGS=-DOS_UNIX

win:
	@make options ${NAME}.exe EXT=.exe OS_FLAGS=-DOS_WIN

${OBJ_PERF} ${OBJ_TEST}:
	${CXX} -c ${TESTS_CXXFLAGS} ${@:.o=.cc} -o $@

tests/perf${EXT}: ${LIB} ${OBJ_PERF}
	${CXX} -o $@ ${OBJ_PERF} ${LIB} ${TESTS_LDFLAGS}

tests/test${EXT}: ${LIB} ${OBJ_TEST}
	${CXX} -o $@ ${OBJ_TEST} ${LIB} ${TESTS_LDFLAGS}

check: tests/perf${EXT} tests/test${EXT}
	cd tests && ./test${EXT} && ./perf${EXT}

clean:
	rm -f ${BIN} ${NAME}.exe ${LIB} ${OBJ} ${OBJ_LIB} ${GMO} data/${NAME}.desktop data/${NAME}.desktop.in.tmp data/${NAME}.metainfo.xml ${NAME}-${VERSION}.tar.gz tests/perf tests/perf.exe tests/perf${EXT} tests/test tests/test.exe tests/test${EXT} ${OBJ_PERF} ${OBJ_TEST}

dist: clean
	mkdir ${NAME}-${VERSION}
	cp -R $$(ls -A | sed '/^\.git$$/d;/^${NAME}-${VERSION}$$/d') ${NAME}-${VERSION}
	tar -cf ${NAME}-${VERSION}.tar ${NAME}-${VERSION}
	gzip ${NAME}-${VERSION}.tar
	rm -fr ${NAME}-${VERSION}

install: data/${NAME}.metainfo.xml ${BIN} data/${NAME}.desktop ${GMO}
	# appstream file
	mkdir -p ${DESTDIR}${PREFIX}/share/metainfo
	cp -f data/${NAME}.metainfo.xml ${DESTDIR}${PREFIX}/share/metainfo
	chmod 644 ${DESTDIR}${PREFIX}/share/metainfo/${NAME}.metainfo.xml
	# bin
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${BIN} ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/${BIN}
	# desktop
	mkdir -p ${DESTDIR}${PREFIX}/share/applications
	cp -f data/${NAME}.desktop ${DESTDIR}${PREFIX}/share/applications
	chmod 644 ${DESTDIR}${PREFIX}/share/applications/${NAME}.desktop
	# doc
	mkdir -p ${DESTDIR}${PREFIX}/share/doc/${NAME}
	cp -f CREDITS.md ${DESTDIR}${PREFIX}/share/doc/${NAME}
	chmod 644 ${DESTDIR}${PREFIX}/share/doc/${NAME}/CREDITS.md
	cp -f LICENSE ${DESTDIR}${PREFIX}/share/doc/${NAME}
	chmod 644 ${DESTDIR}${PREFIX}/share/doc/${NAME}/LICENSE
	cp -f MANUAL.md ${DESTDIR}${PREFIX}/share/doc/${NAME}
	chmod 644 ${DESTDIR}${PREFIX}/share/doc/${NAME}/MANUAL.md
	cp -f README.md ${DESTDIR}${PREFIX}/share/doc/${NAME}
	chmod 644 ${DESTDIR}${PREFIX}/share/doc/${NAME}/README.md
	# icons
	mkdir -p ${DESTDIR}${PREFIX}/share/icons/hicolor/16x16/apps
	cp -f data/icons/16x16/${NAME}.png ${DESTDIR}${PREFIX}/share/icons/hicolor/16x16/apps
	chmod 644 ${DESTDIR}${PREFIX}/share/icons/hicolor/16x16/apps/${NAME}.png
	mkdir -p ${DESTDIR}${PREFIX}/share/icons/hicolor/22x22/apps
	cp -f data/icons/22x22/${NAME}.png ${DESTDIR}${PREFIX}/share/icons/hicolor/22x22/apps
	chmod 644 ${DESTDIR}${PREFIX}/share/icons/hicolor/22x22/apps/${NAME}.png
	mkdir -p ${DESTDIR}${PREFIX}/share/icons/hicolor/24x24/apps
	cp -f data/icons/24x24/${NAME}.png ${DESTDIR}${PREFIX}/share/icons/hicolor/24x24/apps
	chmod 644 ${DESTDIR}${PREFIX}/share/icons/hicolor/24x24/apps/${NAME}.png
	mkdir -p ${DESTDIR}${PREFIX}/share/icons/hicolor/32x32/apps
	cp -f data/icons/32x32/${NAME}.png ${DESTDIR}${PREFIX}/share/icons/hicolor/32x32/apps
	chmod 644 ${DESTDIR}${PREFIX}/share/icons/hicolor/32x32/apps/${NAME}.png
	mkdir -p ${DESTDIR}${PREFIX}/share/icons/hicolor/48x48/apps
	cp -f data/icons/48x48/${NAME}.png ${DESTDIR}${PREFIX}/share/icons/hicolor/48x48/apps
	chmod 644 ${DESTDIR}${PREFIX}/share/icons/hicolor/48x48/apps/${NAME}.png
	mkdir -p ${DESTDIR}${PREFIX}/share/icons/hicolor/scalable/apps
	cp -f data/icons/scalable/${NAME}.svg ${DESTDIR}${PREFIX}/share/icons/hicolor/scalable/apps
	chmod 644 ${DESTDIR}${PREFIX}/share/icons/hicolor/scalable/apps/${NAME}.svg
	# locale
	for gmo in po/*.gmo; do \
		dir="$$(echo $$gmo | sed 's/^po\/\(.*\)\.gmo$$/\1/')"; \
		mkdir -p ${DESTDIR}${PREFIX}/share/locale/$${dir}/LC_MESSAGES; \
		cp -f $$gmo ${DESTDIR}${PREFIX}/share/locale/$${dir}/LC_MESSAGES/${NAME}.mo; \
		chmod 644 ${DESTDIR}${PREFIX}/share/locale/$${dir}/LC_MESSAGES/${NAME}.mo; \
	done;
	# man
	mkdir -p ${DESTDIR}${PREFIX}/share/man/man1
	cp -f man/${NAME}.1 ${DESTDIR}${PREFIX}/share/man/man1
	chmod 644 ${DESTDIR}${PREFIX}/share/man/man1/${NAME}.1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/share/metainfo/${NAME}.metainfo.xml
	rm -f ${DESTDIR}${PREFIX}/bin/${BIN}
	rm -f ${DESTDIR}${PREFIX}/share/applications/${NAME}.desktop
	rm -fr ${DESTDIR}${PREFIX}/share/doc/${NAME}
	rm -f ${DESTDIR}${PREFIX}/share/icons/hicolor/16x16/apps/${NAME}.png
	rm -f ${DESTDIR}${PREFIX}/share/icons/hicolor/22x22/apps/${NAME}.png
	rm -f ${DESTDIR}${PREFIX}/share/icons/hicolor/24x24/apps/${NAME}.png
	rm -f ${DESTDIR}${PREFIX}/share/icons/hicolor/32x32/apps/${NAME}.png
	rm -f ${DESTDIR}${PREFIX}/share/icons/hicolor/48x48/apps/${NAME}.png
	rm -f ${DESTDIR}${PREFIX}/share/icons/hicolor/scalable/apps/${NAME}.svg
	find ${DESTDIR}${PREFIX} -path '${DESTDIR}${PREFIX}/share/locale/*/LC_MESSAGES/${NAME}.mo' -exec rm -f {} \;
	rm -f ${DESTDIR}${PREFIX}/share/man/man1/${NAME}.1

.PHONY:    all options osx unix win check clean install uninstall
.SUFFIXES: .po .gmo
