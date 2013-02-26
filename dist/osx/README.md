# Building the OS X bundle

Using [MacPorts](http://www.macports.org/) install build dependencies:

    port install autoconf automake gcc47 git-core intltool upx yasm.

Download and build wxWidgets 2.9+, example configure flags:

    ./configure --prefix=$HOME/usr --disable-shared \
        --enable-intl --with-osx_cocoa \
        --with-jpeg=builtin --with-png=builtin --with-regex=builtin \
        --with-tiff=builtin --with-zlib=builtin --with-expat=builtin
    cd locale; make allmo; cd ..
    make && make install

Copy the wxWidgets m4 macro to the MacPorts tree:

    sudo cp $HOME/usr/share/aclocal/wxwin.m4 /opt/local/share/aclocal/

Download and build libopus, example configure flags:

    ./configure --prefix=$HOME/usr --disable-shared
    make && make install

Download and build FFmpeg, example configure flags:

    ./configure --prefix=$HOME/usr --enable-gpl --enable-version3 \
        --enable-runtime-cpudetect --enable-pthreads \
        --disable-shared --disable-debug --disable-doc \
        --disable-nonfree --disable-programs --disable-avdevice \
        --disable-swresample --disable-swscale --disable-postproc \
        --disable-avfilter --disable-encoders --disable-muxers \
        --disable-devices --disable-filters \
        --enable-libopus
    make && make install

Bundle Spek:

    ./dist/osx/bundle.sh
