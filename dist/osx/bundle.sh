#!/usr/bin/env bash

set -euo pipefail

LANGUAGES="bs ca cs da de el eo es fi fr gl he hr hu id it ja ko lv nb nl nn pl pt_BR ru sk sr@latin sv th tr uk vi zh_CN zh_TW"

# Discover Homebrew prefix (/usr/local on Intel, /opt/homebrew on Apple Silicon)
# and the installed wxWidgets series (e.g. "3.2", "3.3"), so this script works
# on both architectures and tolerates wx version bumps without hand-edits.
command -v brew >/dev/null 2>&1 || { echo "error: brew not found in PATH" >&2; exit 1; }
command -v wx-config >/dev/null 2>&1 || { echo "error: wx-config not found in PATH" >&2; exit 1; }
BREW_PREFIX=$(brew --prefix)
WX_SERIES=$(wx-config --release)

# Homebrew's wxwidgets ships wxwin.m4 inside its keg, not in the shared
# aclocal dir, so aclocal can't find AM_PATH_WXCONFIG by default. Point at it
# explicitly here instead of asking the user to symlink into Homebrew's tree.
WX_ACLOCAL_DIR="$(brew --prefix wxwidgets)/share/wx/$WX_SERIES/aclocal"
export ACLOCAL_PATH="$WX_ACLOCAL_DIR:${ACLOCAL_PATH:-}"

# Homebrew's gettext is keg-only on macOS, so configure doesn't find libintl
# and silently disables NLS — which then breaks the .gmo copy below. Expose
# gettext's headers and library to configure explicitly.
GETTEXT_PREFIX="$(brew --prefix gettext)"
export CPPFLAGS="-I$GETTEXT_PREFIX/include ${CPPFLAGS:-}"
export LDFLAGS="-L$GETTEXT_PREFIX/lib ${LDFLAGS:-}"

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
cp ../../CREDITS.md Spek.app/Contents/Resources/
cp ../../LICENSE Spek.app/Contents/Resources/
cp ../../README.md Spek.app/Contents/Resources/
mkdir Spek.app/Contents/Resources/lic
cp ../../lic/* Spek.app/Contents/Resources/lic/

for lang in $LANGUAGES; do
    mkdir -p Spek.app/Contents/Resources/"$lang".lproj
    cp -v ../../po/"$lang".gmo Spek.app/Contents/Resources/"$lang".lproj/spek.mo
    cp -v "$BREW_PREFIX"/share/locale/"$lang"/LC_MESSAGES/wxstd-"$WX_SERIES".mo Spek.app/Contents/Resources/"$lang".lproj/ || echo "No WX translation for $lang"
done
mkdir -p Spek.app/Contents/Resources/en.lproj

BINS="Spek.app/Contents/MacOS/Spek"
while [ ! -z "$BINS" ]; do
    NEWBINS=""
    for bin in $BINS; do
        echo "Updating dependendies for $bin."
        # Pick out every linked-library line, then resolve each to a real
        # file under the Homebrew prefix. Handles two reference styles:
        #   - absolute: $BREW_PREFIX/...
        #   - rpath:    @rpath/libfoo.dylib  (e.g. webp -> sharpyuv)
        # System libs (/usr/lib, /System/...) and already-rewritten
        # @executable_path/... entries are skipped.
        LIBS=$(otool -L $bin | tail -n +2 | tr -d '\t' | awk '{print $1}')
        for lib in $LIBS; do
            case "$lib" in
                "$BREW_PREFIX"/*)
                    reallib=$(realpath "$lib")
                    ;;
                @rpath/*)
                    rpath_name="${lib#@rpath/}"
                    if [ -e "$BREW_PREFIX/lib/$rpath_name" ]; then
                        reallib=$(realpath "$BREW_PREFIX/lib/$rpath_name")
                    else
                        echo "\tWarning: cannot resolve $lib referenced from $bin; skipping."
                        continue
                    fi
                    ;;
                *)
                    continue
                    ;;
            esac
            libname=$(basename "$reallib")
            install_name_tool -change "$lib" "@executable_path/../Frameworks/$libname" "$bin"
            if [ ! -f "Spek.app/Contents/Frameworks/$libname" ]; then
                echo "\tBundling $reallib."
                cp "$reallib" "Spek.app/Contents/Frameworks/"
                chmod +w "Spek.app/Contents/Frameworks/$libname"
                install_name_tool -id "@executable_path/../Frameworks/$libname" "Spek.app/Contents/Frameworks/$libname"
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
