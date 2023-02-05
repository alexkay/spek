## Building the Windows installer

This is done in two steps:

 * Cross-compiling spek.exe using [MXE](http://mxe.cc/).
 * Building the MSI package under Windows.

For the first step you can use any Unix-y environment. Set up
[MXE](http://mxe.cc/#tutorial).

Apply `mxe.diff` and build Spek dependencies:

    make pthreads ffmpeg wxwidgets -j8 JOBS=8 MXE_TARGETS='x86_64-w64-mingw32.static'

Build Spek, adjusting `bundle.sh` variables as necessary:

    ./dist/win/bundle.sh

For the second step, you will need a Windows box with
[WiX](http://wixtoolset.org/) installed. Copy over the entire `dist/win`
directory, cd into it, and run `bundle.bat`.
