#!/bin/sh

# This script will cross-compile spek.exe, make a ZIP archive and prepare files
# for building an MSI installer under Windows.
# Check README.md in this directory for instructions.

# Adjust these variables if necessary.
HOST=i686-pc-mingw32
LANGUAGES="cs da de eo es fr it ja nl pl pt_BR ru sv tr uk zh_CN zh_TW"
MAKE=gmake
MXE=$(realpath $(dirname $0)/../../../mxe/usr/i686-pc-mingw32)
STRIP=$(which i686-pc-mingw32-strip)
UPX=$(which upx)
WINDRES=$(which i686-pc-mingw32-windres)
WX_CONFIG="$MXE"/bin/wx-config
ZIP=$(which zip)

cd $(dirname $0)/../..
rm -fr dist/win/build && mkdir dist/win/build

# Compile the resource file
rm -f dist/win/spek.res
"$WINDRES" dist/win/spek.rc -O coff -o dist/win/spek.res
mkdir -p src/dist/win && cp dist/win/spek.res src/dist/win/

# Compile and strip spek.exe
LDFLAGS="dist/win/spek.res" ./configure \
    --host="$HOST" \
    --with-wx-config="$WX_CONFIG" \
    --prefix=${PWD}/dist/win/build && \
    "$MAKE" && \
    "$MAKE" install || exit 1
"$STRIP" dist/win/build/bin/spek.exe
"$UPX" dist/win/build/bin/spek.exe

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
    cp "$MXE"/share/locale/"$lang"/LC_MESSAGES/wxstd.mo Spek/"$lang"/
done
rm -fr build

# Create a zip archive
rm -f spek.zip
"$ZIP" -r spek.zip Spek

cd ../..

# Clean up
rm -fr src/dist
rm dist/win/spek.res
