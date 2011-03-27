#!/usr/bin/env bash

# This script will build a ZIP archive and an MSI installer for Win32.
# It requires a fully functioning MinGW/MSYS environment.
# The script also depends on 7z and WiX.

WIX_PATH=c:/Program\ Files/Windows\ Installer\ XML\ v3.5/bin
SZ_PATH=c:/Program\ Files/7-Zip

pushd $(dirname $0)/../..

rm -fr dist/win/Spek
mkdir dist/win/Spek

# Compile the resource file
windres dist/win/spek.rc -O coff -o dist/win/spek.res
mkdir -p src/dist/win && cp dist/win/spek.res src/dist/win/

CFLAGS="-mwindows" LDFLAGS="dist/win/spek.res" ./configure --prefix=${PWD}/dist/win/Spek && make && make install

cd dist/win/Spek

urls=(\
# GTK+ and its dependencies
"http://ftp.gnome.org/pub/gnome/binaries/win32/atk/1.30/atk_1.30.0-1_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/glib/2.24/glib_2.24.0-2_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/gtk+/2.20/gtk+_2.20.0-1_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/pango/1.28/pango_1.28.0-1_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/cairo_1.8.10-3_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/expat_2.0.1-1_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/fontconfig_2.8.0-2_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/freetype_2.3.12-1_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/gettext-runtime-0.17-1.zip" \
"http://ftp.gnome.org/pub/GNOME/binaries/win32/dependencies/hicolor-icon-theme_0.10-1_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/libpng_1.4.0-1_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/zlib_1.2.4-2_win32.zip" \
# FFmpeg
"http://ffmpeg.arrozcru.org/autobuilds/ffmpeg/mingw32/shared/ffmpeg-r24135-swscale-r31628-mingw32-shared.7z"
)

for url in ${urls[*]}
do
  wget $url
  if [ $? != 0 ]; then
    echo "Can't get $url"
    popd
    exit 1
  fi
  name=$(basename $url)
  "$SZ_PATH"/7z x -y $name
  rm $name
done

# Clean up
mv bin/spek.exe ../
rm -fr share/locale
rm -fr doc
rm -fr presets
rm *.txt
rm bin/avdevice* bin/avfilter* bin/swscale*
rm bin/*.exe
mv licenses share/

# Set the default GTK theme
echo "gtk-theme-name = \"MS-Windows\"" > etc/gtk-2.0/gtkrc

cd ..

# Generate a wxs for files in Spek
"$WIX_PATH"/heat dir Spek -gg -ke -srd -cg Files -dr INSTALLLOCATION -template fragment -o files.wxs

# Make the MSI package
"$WIX_PATH"/candle spek.wxs files.wxs
"$WIX_PATH"/light -ext WixUIExtension.dll -b Spek spek.wixobj files.wixobj -o spek.msi
start fix-msi.js spek.msi

# Clean up
rm *.res
rm *.wixobj
rm *.wixpdb

# Create a zip archive
mv spek.exe Spek/bin/
cp LICENSE.rtf Spek/
cp spek.ico Spek/
"$SZ_PATH"/7z a spek.zip Spek

popd