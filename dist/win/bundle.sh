#!/usr/bin/env bash

set -euo pipefail

# This script will cross-compile spek.exe, make a ZIP archive and prepare files
# for building an MSI installer under Windows.
# Check README.md in this directory for instructions.

# Adjust these variables if necessary.
MXE=$(realpath $(dirname $0)/../../../mxe/usr)
MAKE=make
ZIP=zip

HOST=x86_64-w64-mingw32.static
LANGUAGES="bs ca cs da de el eo es fi fr gl he hr hu id it ja ko lv nb nl nn pl pt_BR ru sk sr@latin sv th tr uk vi zh_CN zh_TW"
PATH="$MXE"/bin:$PATH
WX_CONFIG="$MXE"/"$HOST"/bin/wx-config

cd $(dirname $0)/../..
rm -fr dist/win/build && mkdir dist/win/build

# Compile the resource file
rm -f dist/win/spek.res
$("$WX_CONFIG" --rescomp) dist/win/spek.rc -O coff -o dist/win/spek.res
mkdir -p src/dist/win && cp dist/win/spek.res src/dist/win/
mkdir -p tests/dist/win && cp dist/win/spek.res tests/dist/win/

# Compile spek.exe
LDFLAGS="-mwindows dist/win/spek.res" ./autogen.sh \
    --host="$HOST" \
    --disable-valgrind \
    --with-wx-config="$WX_CONFIG" \
    --prefix=${PWD}/dist/win/build && \
    "$MAKE" -j8 && \
    "$MAKE" install || exit 1

# Compile test.exe
LDFLAGS="-mconsole" ./autogen.sh \
    --host="$HOST" \
    --disable-valgrind \
    --with-wx-config="$WX_CONFIG" \
    --prefix=${PWD}/dist/win/build && \
    "$MAKE" check -j8 TESTS=

# Copy files to the bundle
cd dist/win
rm -fr Spek && mkdir Spek
cp build/bin/spek.exe Spek/
cp ../../CREDITS.md Spek/
cp ../../LICENSE Spek/
cp ../../README.md Spek/
mkdir Spek/lic
cp ../../lic/* Spek/lic/
for lang in $LANGUAGES; do
    mkdir -p Spek/"$lang"
    cp build/share/locale/"$lang"/LC_MESSAGES/spek.mo Spek/"$lang"/
    cp "$MXE"/"$HOST"/share/locale/"$lang"/LC_MESSAGES/wxstd-3.1.mo Spek/"$lang"/ || echo "No WX translation for $lang"
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
