#!/usr/bin/env bash

# This script will build an MSI installer for Win32.
# It requires a fully functioning MinGW/MSYS environment.
# The script also depends on 7z and WiX.

WIX_PATH=c:/Program\ Files/Windows\ Installer\ XML\ v3.5/bin
SZ_PATH=c:/Program\ Files/7-Zip

pushd $(dirname $0)/..

rm -fr win/msi-data
mkdir win/msi-data
CFLAGS="-mwindows" ./configure --prefix=${PWD}/win/msi-data && make && make install

cd win/msi-data

urls=(\
# GTK+ and its dependencies
"http://ftp.gnome.org/pub/gnome/binaries/win32/atk/1.30/atk_1.30.0-1_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/cairo_1.8.10-3_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/expat_2.0.1-1_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/fontconfig_2.8.0-2_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/freetype_2.3.12-1_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/gettext-runtime-0.17-1.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/glib/2.24/glib_2.24.0-2_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/gtk+/2.20/gtk+_2.20.0-1_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/pango/1.28/pango_1.28.0-1_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/libpng_1.4.0-1_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/libxml2_2.7.7-1_win32.zip" \
"http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/zlib_1.2.4-2_win32.zip" \
# GStreamer dependencies
"http://ossbuild.googlecode.com/svn/trunk/Shared/Build/Windows/Win32/bin/avcodec-gpl-52.dll" \
"http://ossbuild.googlecode.com/svn/trunk/Shared/Build/Windows/Win32/bin/avformat-gpl-52.dll" \
"http://ossbuild.googlecode.com/svn/trunk/Shared/Build/Windows/Win32/bin/avutil-gpl-50.dll" \
"http://ossbuild.googlecode.com/svn/trunk/Shared/Build/Windows/Win32/bin/libbz2.dll" \
"http://ossbuild.googlecode.com/svn/trunk/Shared/Build/Windows/Win32/bin/libfaac-0.dll" \
"http://ossbuild.googlecode.com/svn/trunk/Shared/Build/Windows/Win32/bin/libfaad-2.dll" \
"http://ossbuild.googlecode.com/svn/trunk/Shared/Build/Windows/Win32/bin/libFLAC-8.dll" \
"http://ossbuild.googlecode.com/svn/trunk/Shared/Build/Windows/Win32/bin/libogg-0.dll" \
"http://ossbuild.googlecode.com/svn/trunk/Shared/Build/Windows/Win32/bin/libvorbis-0.dll" \
"http://ossbuild.googlecode.com/svn/trunk/Shared/Build/Windows/Win32/bin/libvorbisenc-2.dll" \
"http://ossbuild.googlecode.com/svn/trunk/Shared/Build/Windows/Win32/bin/z.dll"
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
  case $name in
    *.dll) mv $name bin/;;
    *) "$SZ_PATH"/7z x -y $name; rm $name;;
  esac
done

# Clean up
rm -fr share/locale
mv x86-OSSBuild-GStreamer-Libraries.msm ..
mv x86-OSSBuild-GStreamer-Plugins-Base.msm ..
mv x86-OSSBuild-GStreamer-Plugins-Good.msm ..
mv x86-OSSBuild-GStreamer-Plugins-Ugly-GPL.msm ..
rm *.msm

# Set the default GTK theme
echo "gtk-theme-name = \"MS-Windows\"" > etc/gtk-2.0/gtkrc

cd ..
#TODO:
# * Desktop and Programs shortcuts
# * Figure out why icons don't work, may be install hi-color icon theme?
# * Uninstall shortcut
# * Website link doesn't work

# Generate a wxs for files in msi-data
"$WIX_PATH"/heat.exe dir msi-data -gg -srd -cg Files -dr INSTALLLOCATION -template fragment -o files.wxs || (popd; exit 1)

# Make the MSI package
"$WIX_PATH"/candle.exe spek.wxs files.wxs || (popd; exit 1)
"$WIX_PATH"/light.exe -ext WixUIExtension.dll -b msi-data spek.wixobj files.wixobj -o spek.msi || (popd; exit 1)

popd