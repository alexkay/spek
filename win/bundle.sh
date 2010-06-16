#!/usr/bin/env bash

# This script will build a ZIP archive and an MSI installer for Win32.
# It requires a fully functioning MinGW/MSYS environment.
# The script also depends on 7z and WiX.

WIX_PATH=c:/Program\ Files/Windows\ Installer\ XML\ v3.5/bin
SZ_PATH=c:/Program\ Files/7-Zip

pushd $(dirname $0)/..

rm -fr win/Spek
mkdir win/Spek

# Compile the resource file
windres win/spek.rc -O coff -o win/spek.res
mkdir src/win && cp win/spek.res src/win/

CFLAGS="-mwindows" LDFLAGS="win/spek.res" ./configure --prefix=${PWD}/win/Spek && make && make install

cd win/Spek

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
"http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/libxml2_2.7.7-1_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/zlib_1.2.4-2_win32.zip" \
# GStreamer merge modules
"http://ossbuild.googlecode.com/files/GStreamer-WinBuilds-Merge-Modules-x86.zip"
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
rm -fr share/locale
mv x86-OSSBuild-GStreamer-Dependencies-GPL.msm ..
mv x86-OSSBuild-GStreamer-Libraries.msm ..
mv x86-OSSBuild-GStreamer-Plugins-Bad-GPL.msm ..
mv x86-OSSBuild-GStreamer-Plugins-Base.msm ..
mv x86-OSSBuild-GStreamer-Plugins-FFmpeg-GPL.msm ..
mv x86-OSSBuild-GStreamer-Plugins-Good.msm ..
mv x86-OSSBuild-GStreamer-Plugins-Ugly-GPL.msm ..
rm *.msm

# Set the default GTK theme
echo "gtk-theme-name = \"MS-Windows\"" > etc/gtk-2.0/gtkrc

cd ..

# Extract files from x86-OSSBuild-GStreamer-Dependencies-GPL.msm
mkdir deps
"$WIX_PATH"/dark x86-OSSBuild-GStreamer-Dependencies-GPL.msm -o deps/deps.wxs -x deps

for line in $(grep "<File" deps/deps.wxs | sed -e "s/.* Name=\"\([^\"]*\)\".* Source=\"\([^\"]*\)\".*/\1;\2/g"); do
  line=${line//\\/\/}
  name=${line%;*}
  src=${line#*;}
  dst="Spek/bin/$name"
  # Move but don't overwrite the existing file
  if [ ! -f "$dst" ] ; then
    mv -v "$src" "$dst"
  fi
done

# Create a zip archive
"$SZ_PATH"/7z a spek.zip Spek

# Generate a wxs for files in Spek
"$WIX_PATH"/heat dir Spek -gg -ke -srd -cg Files -dr INSTALLLOCATION -template fragment -o files.wxs

# Make the MSI package
"$WIX_PATH"/candle spek.wxs files.wxs
"$WIX_PATH"/light -ext WixUIExtension.dll -b Spek spek.wixobj files.wixobj -o spek.msi

# Clean up
rm -fr deps
rm *.msm
rm *.wixobj

popd