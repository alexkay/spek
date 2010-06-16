#!/bin/sh

# This script creates a Mac OS X app bundle and a DMG image from it
# using jhbuild/ige-mac-bundler. JHBuild scripts from the GTK-OSX
# project should be installed and properly configured.
#
# Also, the script assumes that all dependencies have already been built,
# including all (not so optional) GStreamer dependencies. GStreamer plugins
# will happily compile even if essential components such as libmad, libvorbis,
# flac, etc are missing. Make sure you have everything that you need installed.
#
# Example build sequence:
#
# $ jhbuild bootstrap
# $ jhbuild build
#
# Download GStreamer dependencies:
#   BASE: libogg, libvorbis, libtheora
#   GOOD: speex, flac, taglib, wavpack
#   BAD: orc, schroedinger, faad2, libdca, libmodplug, libmpcdec, xvid
#   UGLY: a52dec, libid3tag, libmad, libmpeg2
#
# Install each one with:
#
# $ jhbuild shell
# $ ./configure --prefix=$PREFIX && make && make install
# $ exit
#
# Install GStreamer:
#
# $ jhbuild build meta-gstreamer
# $ jhbuild build gst-plugins-ugly
#
# IMPORTANT: the script must be run from the `jhbuild shell` environment.

pushd $(dirname $0)/..

# Build Spek
./configure --prefix=$PREFIX && make && make install

# Bundle
cd osx
ige-mac-bundler spek.bundle

# Make DMG image
#./make-dmg.sh spek 0.4 spek.app
VOLUME_NAME=Spek
DMG_APP=Spek.app
DMG_FILE=$VOLUME_NAME.dmg
MOUNT_POINT=$VOLUME_NAME.mounted

rm -f $DMG_FILE
rm -f $DMG_FILE.master

# Compute an approximated image size in MB, and bloat by 1MB
image_size=$(du -ck $DMG_APP dmg-data | tail -n1 | cut -f1)
image_size=$((($image_size + 5000) / 1000))

echo "Creating disk image (${image_size}MB)..."
hdiutil create $DMG_FILE -megabytes $image_size -volname $VOLUME_NAME -fs HFS+ -quiet || exit $?

echo "Attaching to disk image..."
hdiutil attach $DMG_FILE -readwrite -noautoopen -mountpoint $MOUNT_POINT -quiet

echo "Populating image..."

cp -Rp $DMG_APP $MOUNT_POINT

find $MOUNT_POINT -type d -iregex '.*\.svn$' &>/dev/null | xargs rm -rf

pushd $MOUNT_POINT &>/dev/null
ln -s /Applications " "
popd &>/dev/null

#mkdir $MOUNT_POINT/.background
#cp dmg-data/background.png $MOUNT_POINT/.background
cp dmg-data/DS_Store $MOUNT_POINT/.DS_Store

echo "Detaching from disk image..."
hdiutil detach $MOUNT_POINT -quiet

mv $DMG_FILE $DMG_FILE.master

echo "Creating distributable image..."
hdiutil convert -quiet -format UDBZ -o $DMG_FILE $DMG_FILE.master

#echo "Installing end user license agreement..."
#hdiutil flatten -quiet $DMG_FILE
#/Developer/Tools/Rez /Developer/Headers/FlatCarbon/*.r dmg-data/license.r -a -o $DMG_FILE
#hdiutil unflatten -quiet $DMG_FILE

echo "Done."

if [ ! "x$1" = "x-m" ]; then
	rm $DMG_FILE.master
fi

popd &>/dev/null