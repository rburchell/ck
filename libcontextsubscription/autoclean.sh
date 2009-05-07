#!/bin/sh -e

make maintainer-clean || true
echo Cleaning autotools files...
find -type d -name autom4te.cache -print0 | xargs -0 rm -rf \;
find -type f \( -name missing -o -name install-sh -o -name mkinstalldirs \
	-o -name depcomp -o -name ltmain.sh -o -name configure \
	-o -name config.sub -o -name config.guess -o -name aclocal.m4 \
	-o -name Makefile.in \) -print0 | xargs -0 rm -f
# rm -f debian-devel/control debian-devel/rules install-stamp
