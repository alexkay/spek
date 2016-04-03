% SPEK(1) User's Guide | Version 0.8.2
%
% 2013-04-18

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

`f`, `F`
:   Change the DFT window function.

`l`, `L`
:   Change the lower limit of the dynamic range in dBFS.

`p`, `P`
:   Change the palette.

`u`, `U`
:   Change the upper limit of the dynamic range in dBFS.

`w`, `W`
:   Change the DFT window size.

# FILES

*~/.config/spek/preferences*
:   The configuration file for *Spek*, stored in a simple INI format.

# AUTHORS

Alexander Kojevnikov <alexander@kojevnikov.com>. Other contributors are listed
in the LICENCE.md file distributed with the source code.

# DISTRIBUTION

The latest version of *Spek* may be downloaded from <http://spek.cc>.
