#!/bin/sh -e

cd $(dirname $0)
autoreconf -i -f -v

# Autoreconf overwrites INSTALL, but we have our own version of it.
cp INSTALL.real INSTALL

# Libtool 2.2 still reorders LDFLAGS badly, and they won't fix it before libtool 2.3 or 2.4
# but we can leave without this
if cat ltmain.sh.as-needed.patch | patch -t -p0 --dry-run
then
	cat ltmain.sh.as-needed.patch | patch -t -p0
fi

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

