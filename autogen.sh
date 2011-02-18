#!/bin/sh
# Run this to generate all the initial makefiles, etc.

test -n "$srcdir" || srcdir=$(dirname "$0")
test -n "$srcdir" || srcdir=.
(
  cd "$srcdir" &&
  AUTOPOINT='intltoolize --automake --copy' autoreconf -fiv -Wall
) || exit
test -n "$NOCONFIGURE" || "$srcdir/configure" --enable-maintainer-mode "$@"