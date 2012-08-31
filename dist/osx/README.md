# Building the OS X bundle

Using [MacPorts](http://www.macports.org/) install build dependencies:

    port install git-core autoconf automake intltool upx yasm.

Download and build wxWidgets 2.9+, example configure flags:

    ./configure --prefix=$HOME/usr --disable-shared \
        --enable-intl --with-osx_cocoa \
        --with-jpeg=builtin --with-png=builtin --with-regex=builtin \
        --with-tiff=builtin --with-zlib=builtin --with-expat=builtin
    cd locale; make allmo; cd ..
    make && make install

Copy the wxWidgets m4 macro to the MacPorts tree:

    sudo cp $HOME/usr/share/aclocal/wxwin.m4 /opt/local/share/aclocal/

Download and build FFmpeg, example configure flags:

    ./configure  --prefix=$HOME/usr --enable-gpl --enable-version3 \
        --enable-runtime-cpudetect --enable-pthreads \
        --disable-shared --disable-debug --disable-doc \
        --disable-nonfree --disable-ffmpeg --disable-ffplay \
        --disable-ffprobe --disable-ffserver --disable-avdevice --disable-swscale \
        --disable-postproc --disable-encoders --disable-muxers \
        --disable-devices --disable-filters --disable-swresample
    make && make install

Clone and build Spek, example configure flags:

    PKG_CONFIG_PATH=$HOME/usr/lib/pkgconfig ./autogen.sh --with-wx-config=$HOME/usr/bin/wx-config

Bundle Spek:

    ./dist/osx/bundle.sh
