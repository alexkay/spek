#!/bin/sh

# This script will cross-compile spek.exe, make a ZIP archive and prepare files
# for building an MSI installer under Windows.
# Check README.md in this directory for instructions.

# Adjust these variables if necessary.
MXE=$(realpath $(dirname $0)/../../../mxe/usr)
MAKE=gmake
UPX=upx
ZIP=zip

HOST=i686-pc-mingw32
LANGUAGES="ca cs da de el eo es fi fr gl it ja nb nl pl pt_BR ru sk sr@latin sv tr uk vi zh_CN zh_TW"
PATH="$MXE"/bin:$PATH
STRIP=i686-pc-mingw32-strip
WINDRES=i686-pc-mingw32-windres
WX_CONFIG="$MXE"/i686-pc-mingw32/bin/wx-config

cd $(dirname $0)/../..
rm -fr dist/win/build && mkdir dist/win/build

# Compile the resource file
rm -f dist/win/spek.res
"$WINDRES" dist/win/spek.rc -O coff -o dist/win/spek.res
mkdir -p src/dist/win && cp dist/win/spek.res src/dist/win/
mkdir -p tests/dist/win && cp dist/win/spek.res tests/dist/win/

# Compile and strip spek.exe
LDFLAGS="-mwindows dist/win/spek.res" ./autogen.sh \
    --host="$HOST" \
    --disable-valgrind \
    --with-wx-config="$WX_CONFIG" \
    --prefix=${PWD}/dist/win/build && \
    "$MAKE" -j8 && \
    "$MAKE" install || exit 1
"$STRIP" dist/win/build/bin/spek.exe
"$UPX" dist/win/build/bin/spek.exe

# Compile test.exe
LDFLAGS="-mconsole" ./autogen.sh \
    --host="$HOST" \
    --disable-valgrind \
    --with-wx-config="$WX_CONFIG" \
    --prefix=${PWD}/dist/win/build && \
    "$MAKE" check -j8

# Copy files to the bundle
cd dist/win
rm -fr Spek && mkdir Spek
cp build/bin/spek.exe Spek/
cp ../../LICENCE.md Spek/
cp ../../README.md Spek/
mkdir Spek/lic
cp ../../lic/* Spek/lic/
for lang in $LANGUAGES; do
    mkdir -p Spek/"$lang"
    cp build/lib/locale/"$lang"/LC_MESSAGES/spek.mo Spek/"$lang"/
    cp "$MXE"/i686-pc-mingw32/share/locale/"$lang"/LC_MESSAGES/wxstd.mo Spek/"$lang"/
done
rm -fr build

# Copy tests
rm -fr tests && mkdir tests
cp ../../tests/.libs/test.exe tests/
cp -a ../../tests/samples tests/

# Create a zip archive
rm -f spek.zip
"$ZIP" -r spek.zip Spek

cd ../..

# Clean up
rm -fr src/dist tests/dist
rm dist/win/spek.res
