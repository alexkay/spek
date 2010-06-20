#!/bin/sh

# This script creates a Mac OS X app bundle and a DMG image from it using bockbuild.

# TODO: create these packages for GStreamer:
#   BAD: orc, schroedinger, faad2, libdca, libmodplug, libmpcdec, xvid
#   UGLY: a52dec, libmpeg2

# TODO:
# - DS_Store
# - Check which gtkrc other projects are using

pushd $(dirname $0)

case "$(uname)" in
	Darwin) profile_name=darwin ;;
	*)
		echo "Unsupported system type: $(uname)"
		exit 1
		;;
esac

# Build Spek and all its dependencies
pushd bockbuild/profiles/spek
./profile."$profile_name".py -bvr

# Build Solitary
pushd ../../solitary
make
popd

# Bundle
./profile."$profile_name".py -z

# Make DMG image
cd bundle."$profile_name"
VOLUME_NAME=Spek
DMG_APP=Spek.app
DMG_FILE=$VOLUME_NAME.dmg
MOUNT_POINT=$VOLUME_NAME.mounted

rm -f $DMG_FILE
rm -f $DMG_FILE.master

# Compute an approximated image size in MB, and bloat by 1MB
image_size=$(du -ck $DMG_APP | tail -n1 | cut -f1)
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
