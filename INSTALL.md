# Installation instructions

## Windows

Download section offers two packages: an MSI installer and a ZIP archive. To
install Spek, download the MSI installer, double-click it and follow the
instructions.

If you don't want to use the installer, you can download the ZIP archive, unpack
it somewhere on your disk and run `Spek\spek.exe`.

## Mac OS X

Spek for Mac OS X is available in the download section. Download and open the
DMG package, then drag the Spek icon to Applications.

Spek requires OS X 10.5+ and an Intel-based Mac.

## *BSD and GNU/Linux

### Binary packages

 * Arch: [spek](https://aur.archlinux.org/packages/spek/) and
   [spek-git](https://aur.archlinux.org/packages/spek-git/)
 * Debian: [spek](https://packages.debian.org/search?keywords=spek)
 * Fedora: [RPMFusion package](https://bugzilla.rpmfusion.org/show_bug.cgi?id=1718)
 * FreeBSD: [audio/spek](https://www.freshports.org/audio/spek/)
 * Gentoo: [media-sound/spek](https://packages.gentoo.org/packages/media-sound/spek)
 * Ubuntu: [spek](http://packages.ubuntu.com/search?keywords=spek)

### Building from the tarball

To build Spek, download the source code tarball then run these commands from
terminal:

    tar -xvf spek-0.8.4.tar.xz
    cd spek-0.8.4
    ./configure
    make

To build you will need wxWidgets and FFmpeg packages. On Debian/Ubuntu you also
need development packages: `libwxgtk2.8-dev`, `wx-common`, `libavcodec-dev` and
`libavformat-dev`.

To start Spek, run:

    src/spek

Or install it with:

    sudo make install

### Building from the git repository

    git clone git://github.com/alexkay/spek.git
    cd spek
    ./autogen.sh
    make
