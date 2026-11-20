% SPEK(1) User's Guide | Version 0.8.5
%
% 2023-01-09

# NAME

Spek - Acoustic Spectrum Analyser

# SYNOPSIS

`spek` [*OPTION* *...*] \[*FILE*]

# DESCRIPTION

*Spek* generates a spectrogram for the input audio file.

# OPTIONS

`-h`, `--help`
:   Output the help message then quit.

`-V`, `--version`
:   Output version information then quit.

# KEYBINDINGS

## Notes

On OS X use the Command key instead of Ctrl.

## Menu

`Ctrl-O`
:   Open a new file.

`Ctrl-S`
:   Save the spectrogram as an image file.

`Ctrl-E`
:   Show the preferences dialog.

`F1`
:   Open online manual in the browser.

`Shift-F1`
:   Show the about dialog.

## Spectrogram

The mouse wheel zooms the time axis around the pointer. Hold Shift while using
the mouse wheel to pan horizontally, or drag the spectrogram with the left mouse
button. When zoomed in, drag the overview strip to move through the full track.
The strip shows the visible range and current zoom factor. Double-click the spectrogram to show the full track again.

`c`, `C`
:   Change the audio channel.

`f`, `F`
:   Change the DFT window function.

`l`, `L`
:   Change the lower limit of the dynamic range in dBFS.

`p`, `P`
:   Change the palette.

`s`, `S`
:   Change the audio stream.

`u`, `U`
:   Change the upper limit of the dynamic range in dBFS.

`w`, `W`
:   Change the DFT window size.

# FILES

*~/.config/spek/preferences*
:   The configuration file for *Spek*, stored in a simple INI format.

# AUTHORS

Alexander Kojevnikov <alexander@kojevnikov.com>. Other contributors are listed
in the CREDITS.md file distributed with the source code.

# DISTRIBUTION

The latest version of *Spek* may be downloaded from <https://www.spek.cc>.
