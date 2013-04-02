#!/bin/sh

INSTALL_PATH=$HOME/usr
LANGUAGES="ca cs da de eo es fr gl it ja nb nl pl pt_BR ru sr@latin sv tr uk vi zh_CN zh_TW"

cd $(dirname $0)/../..

rm -f src/spek

PKG_CONFIG_PATH=$INSTALL_PATH/lib/pkgconfig CXX=/opt/local/bin/g++-mp-4.7 \
    ./autogen.sh --with-wx-config=$INSTALL_PATH/bin/wx-config \
    && make || exit 1
strip src/spek
upx src/spek

cd dist/osx
rm -fr Spek.app
mkdir -p Spek.app/Contents/MacOS
mkdir -p Spek.app/Contents/Resources
mv ../../src/spek Spek.app/Contents/MacOS/Spek
cp Info.plist Spek.app/Contents/
cp Spek.icns Spek.app/Contents/Resources/
cp *.png Spek.app/Contents/Resources/
cp ../../LICENCE.md Spek.app/Contents/Resources/
cp ../../README.md Spek.app/Contents/Resources/
mkdir Spek.app/Contents/Resources/lic
cp ../../lic/* Spek.app/Contents/Resources/lic/

for lang in $LANGUAGES; do
    mkdir -p Spek.app/Contents/Resources/"$lang".lproj
    cp -v ../../po/"$lang".gmo Spek.app/Contents/Resources/"$lang".lproj/spek.mo
    cp -v "$INSTALL_PATH"/share/locale/"$lang"/LC_MESSAGES/wxstd.mo Spek.app/Contents/Resources/"$lang".lproj/
done
mkdir -p Spek.app/Contents/Resources/en.lproj

# Make DMG image
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
cd $MOUNT_POINT
ln -s /Applications " "
cd ..
cp DS_Store $MOUNT_POINT/.DS_Store

echo "Detaching from disk image..."
hdiutil detach $MOUNT_POINT -quiet
mv $DMG_FILE $DMG_FILE.master

echo "Creating distributable image..."
hdiutil convert -quiet -format UDBZ -o $DMG_FILE $DMG_FILE.master
rm $DMG_FILE.master

echo "Done."

cd ../..
