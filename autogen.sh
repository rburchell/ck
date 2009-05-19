#!/bin/sh
set -e

. tools/autogen-helpers.sh

version_check valac VALAC 'valac' 0.7.1 "http://vala-project.org"
version_check asciidoc ASCIIDOC 'asciidoc' 8.2.7 "http://www.methods.co.nz/asciidoc/"
existence_check gtkdocize   
existence_check dot
existence_check source-highlight

gtkdocize

# Autoreconf will overwrite INSTALL, but we have our own version of
# it.

autoreconf -i -f
cp INSTALL.real INSTALL

run_configure=true
for arg in $*; do
    case $arg in
        --no-configure)
            run_configure=false
            ;;
        *)
            ;;
    esac
done

if test $run_configure = true; then
    ./configure "$@"
fi

