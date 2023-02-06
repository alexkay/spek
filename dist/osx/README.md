# Building the OS X bundle

Using [Homebrew](http://brew.sh) install build dependencies:

    brew install automake coreutils git
    brew install ffmpeg wxwidgets

Add a missing symlink:

    (cd /usr/local/share/aclocal && ln -s ../../Cellar/wxwidgets/3.2.1/share/wx/3.2/aclocal/wxwin.m4 .)

Fix libbrotli loader_path:

    install_name_tool -change "@loader_path/libbrotlicommon.1.dylib" "/usr/local/lib/libbrotlicommon.1.dylib" /usr/local/lib/libbrotlidec.1.dylib
    install_name_tool -change "@loader_path/libbrotlicommon.1.dylib" "/usr/local/lib/libbrotlicommon.1.dylib" /usr/local/lib/libbrotlienc.1.dylib

Bundle Spek:

    ./dist/osx/bundle.sh
