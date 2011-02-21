#!/bin/sh

cd $(dirname $0)/..
case "$(uname)" in
    Darwin) profile_name=darwin ;;
    *)
        echo "Unsupported system type: $(uname)"
        exit 1
        ;;
esac

./autogen.sh --prefix=$(pwd)/osx/bin
gmake
gmake install

ige-mac-bundler osx/spek.bundle

# Make DMG image
cd osx
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

#pushd $MOUNT_POINT &>/dev/null

cd ..
