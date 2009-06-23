#!/bin/sh -e

cd $(dirname $0)
autoreconf -i -f -v

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

