#!/bin/sh

LANGUAGES="bs ca cs da de el eo es fi fr gl he hr hu id it ja ko lv nb nl nn pl pt_BR ru sk sr@latin sv th tr uk vi zh_CN zh_TW"

cd $(dirname $0)/../..

rm -f src/spek

./autogen.sh && make -j8 || exit 1

cd dist/osx
rm -fr Spek.app
mkdir -p Spek.app/Contents/MacOS
mkdir -p Spek.app/Contents/Frameworks
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
    cp -v /usr/local/share/locale/"$lang"/LC_MESSAGES/wxstd.mo Spek.app/Contents/Resources/"$lang".lproj/
done
mkdir -p Spek.app/Contents/Resources/en.lproj

BINS="Spek.app/Contents/MacOS/Spek"
while [ ! -z "$BINS" ]; do
    NEWBINS=""
    for bin in $BINS; do
        echo "Updating dependendies for $bin."
        LIBS=$(otool -L $bin | grep /usr/local | tr -d '\t' | awk '{print $1}')
        for lib in $LIBS; do
            reallib=$(realpath $lib)
            libname=$(basename $reallib)
            install_name_tool -change $lib @executable_path/../Frameworks/$libname $bin
            if [ ! -f Spek.app/Contents/Frameworks/$libname ]; then
                echo "\tBundling $reallib."
                cp $reallib Spek.app/Contents/Frameworks/
                chmod +w Spek.app/Contents/Frameworks/$libname
                install_name_tool -id @executable_path/../Frameworks/$libname Spek.app/Contents/Frameworks/$libname
                NEWBINS="$NEWBINS Spek.app/Contents/Frameworks/$libname"
            fi
        done
    done
    BINS="$NEWBINS"
done

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
