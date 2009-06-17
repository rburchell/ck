#!/bin/sh -e

cd $(dirname $0)

. tools/autogen-helpers.sh

# fixme: can we move these four out of here?
version_check valac VALAC 'valac' 0.7.1 "http://vala-project.org"
version_check asciidoc ASCIIDOC 'asciidoc' 8.2.7 "http://www.methods.co.nz/asciidoc/"
existence_check dot
existence_check source-highlight

existence_check gtkdocize   
gtkdocize

autoreconf -i -f

# Autoreconf overwrites INSTALL, but we have our own version of it.
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

