# Spek

Spek is an acoustic spectrum analyser written in C and C++. It uses wxWidgets
for the GUI and FFmpeg libraries for audio decoding.

Spek is available on *BSD, GNU/Linux, Windows and Mac OS X.

Find out more about Spek on its website: <http://www.spek-project.org/>

## Spek 0.7 - Released 2011-04-24

### New Features And Enhancements

Spek 0.7 is part of beta 0.x releases, leading up to a stable 1.0 release
later this year. Check Spek website for the roadmap.

New features since 0.6:

 * Added translations in 8 languages
 * Preferences to select a language and to check for a new version
 * Spectral density ruler
 * Added a menu bar, cleaned up the tool bar.
 * Better OS X integration

Enhancements:

 * Show the version number in the window
 * Pre-compute the cosine table to speed up analysis by ±16%
 * Use jhbuild and ige-mac-bundler to build and package Spek on OS X
 * spek(1) man page
 * Avoid using APIs depreciated in GTK3

Bugfixes:

 * Fixed link activation on OS X (issue 31)
 * Fixed new version detection on OS X
 * Fixed duration for unsynchronised ID3v24 mp3 tags (upstream fix)
 * Fixed rigth click → Quit (issue 24) and the ⌘ Q shortcut (issue 44)

### Sources / Packages

 * Spek 0.7 tarball:
   * <http://spek.googlecode.com/files/spek-0.7.tar.bz2>

 * Windows and Mac OS X binaries:
   * <http://spek.googlecode.com/files/spek-0.7.msi>
   * <http://spek.googlecode.com/files/spek-0.7.zip>
   * <http://spek.googlecode.com/files/spek-0.7.dmg>

 * Unix packages:
   * <https://github.com/alexkay/spek/blob/master/INSTALL.md>

### Dependencies

 * wxWidgets >= 2.8
 * FFmpeg libraries:
   * libavcodec >= 52.123
   * libavformat >= 52.111
   * libavutil
