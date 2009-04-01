#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="contextd"

REQUIRED_AUTOMAKE_VERSION=1.9

. gnome-autogen.sh

version_check valac VALAC 'valac' 0.7.0 "http://vala-project.org"
