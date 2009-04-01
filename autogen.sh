#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="contextd"

REQUIRED_AUTOMAKE_VERSION=1.9

. gnome-autogen.sh

existence_check () {
  printbold checking for $1...
  if ! which $1 >/dev/null; then
    printerr "***Error***: You must have $1 installed to hack on $PKG_NAME."
  fi
}

version_check valac VALAC 'valac' 0.7.0 "http://vala-project.org"
version_check asciidoc ASCIIDOC 'asciidoc' 8.2.7 "http://www.methods.co.nz/asciidoc/"
existence_check dot
existence_check source-highlight
