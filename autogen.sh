#!/bin/sh
set -e

. tools/autogen-helpers.sh

version_check valac VALAC 'valac' 0.7.0 "http://vala-project.org"
version_check asciidoc ASCIIDOC 'asciidoc' 8.2.7 "http://www.methods.co.nz/asciidoc/"
existence_check dot
existence_check source-highlight

autoreconf -i -f

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

