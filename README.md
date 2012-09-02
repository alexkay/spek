# Spek

Spek is an acoustic spectrum analyser written in C and C++. It uses wxWidgets
for the GUI and FFmpeg libraries for audio decoding.

Spek is available on *BSD, GNU/Linux, Windows and Mac OS X.

Find out more about Spek on its website: <http://spek-project.org/>

## Spek 0.8.0 - Released 2012-TODO

### New Features And Enhancements

Spek 0.8.0 is part of beta 0.x releases, leading up to a stable 1.0 release
(hopefully) later this year. Check Spek website for the roadmap.

New features since 0.7:

 * Switched from GTK+ to wxWidgets for better Windows and OS X integration.
 * Single .exe version for Windows.
 * Added translations in 8 more languages (totalling 16).

Enhancements:

 * Switched to .xz tarballs.
 * Split out libspek and added unit tests.
 * 24-bit APE support (upstream fix).

Bugfixes:

 * Fixed crash when the preferences file is not writable.
 * Fixed crash when the home directory is not writable.
 * Fixed duration and rendering for some video files.
 * Fixed compilation with newer FFmpeg and libav versions.
 * Proper handling of Unicode file names under Windows (upstream fix).

### Sources / Packages

 * Spek 0.8.0 tarball:
   * <https://github.com/downloads/alexkay/spek/spek-0.8.0.tar.xz>

 * Windows and Mac OS X binaries:
   * <https://github.com/downloads/alexkay/spek/spek-0.8.0.msi>
   * <https://github.com/downloads/alexkay/spek/spek-0.8.0.zip>
   * <https://github.com/downloads/alexkay/spek/spek-0.8.0.dmg>

 * Unix packages:
   * <https://github.com/alexkay/spek/blob/master/INSTALL.md#bsd-and-gnulinux>

### Dependencies

 * wxWidgets >= 2.8
 * FFmpeg >= 0.7 or libav >= 0.8
