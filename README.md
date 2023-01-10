# Spek

Spek is an acoustic spectrum analyser written in C++. It uses FFmpeg
libraries for audio decoding and wxWidgets for the GUI.

Spek is available on *BSD, GNU/Linux, Windows and Mac OS X.

Find out more about Spek on its website: <http://spek.cc/>

## Spek 0.8.5 - Released 2023-01-09

### New Features And Enhancements

New features since 0.8.2:

 * Upgrade to FFmpeg 5.1
 * Add 2 more palettes and change the default.
 * Allow changing the DFT window size and function.
 * Allow switching between audio streams and channels.
 * Add translations for 14 additional languages.

Enhancements:

 * Remove dependency on `intltool`.
 * Fix FFmpeg build warnings.
 * Detect AR tool.
 * Use Homebrew for macOS dependencies.
 * Improve test coverage.
 * Use `XDG_CONFIG_HOME` on Unix systems.

Bugfixes:

 * Remove association with .mod and MIDI files.
 * Fix autoconf errors.
 * Fix an AVX-related crash.

### Sources / Packages

Spek 0.8.5 tarball:

 * <https://github.com/alexkay/spek/releases/download/v0.8.5/spek-0.8.5.tar.xz>

Windows and Mac OS X binaries:

 * <https://github.com/alexkay/spek/releases/download/v0.8.2/spek-0.8.2.msi>
 * <https://github.com/alexkay/spek/releases/download/v0.8.2/spek-0.8.2.zip>
 * <https://github.com/alexkay/spek/releases/download/v0.8.2/spek-0.8.2.dmg>

Unix packages:

 * <https://github.com/alexkay/spek/blob/master/INSTALL.md#bsd-and-gnulinux>

### Dependencies

 * wxWidgets >= 3
 * A recent version of FFmpeg
