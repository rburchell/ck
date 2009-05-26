#!/bin/bash -e

# Remove all temp files
function clean {
    `rm -f *.actual`
    `rm -f *.cdb`
}

# Compare file $1 to $2 and display diff if different
function compare {
    f=`diff -u "$1" "$2"`
    if [ $? != 0 ] ; then
        echo "$1 and $2 differ!"
        echo "$f"
        clean
        exit 128
    fi
}

# Query the database $1 using the cdb tool and echo results to $1.actual
function querydb {
    cdb -q "context-providers.cdb" "$1" > "$1.actual"
}

# Check the keys in the existing context-providers.cdb database
function dotest {
    querydb "KEYS"
    compare "KEYS.expected" "KEYS.actual"

    querydb "PROVIDERS"
    compare "PROVIDERS.expected" "PROVIDERS.actual"

    querydb "org.freedesktop.ContextKit.contextd1:KEYS"
    compare "org.freedesktop.ContextKit.contextd1:KEYS.expected" "org.freedesktop.ContextKit.contextd1:KEYS.actual"

    querydb "org.freedesktop.ContextKit.contextd2:KEYS"
    compare "org.freedesktop.ContextKit.contextd2:KEYS.expected" "org.freedesktop.ContextKit.contextd2:KEYS.actual"

    querydb "Battery.Charging:KEYTYPE"
    compare "Battery.Charging:KEYTYPE.expected" "Battery.Charging:KEYTYPE.actual"

    querydb "Battery.Charging:KEYDOC"
    compare "Battery.Charging:KEYDOC.expected" "Battery.Charging:KEYDOC.actual"

    querydb "Battery.Charging:KEYPROVIDER"
    compare "Battery.Charging:KEYPROVIDER.expected" "Battery.Charging:KEYPROVIDER.actual"

    querydb "Battery.Charging:KEYBUS"
    compare "Battery.Charging:KEYBUS.expected" "Battery.Charging:KEYBUS.actual"
}

BASEDIR=`dirname $0`

# Test using command line param
../../update-contextkit-providers/update-contextkit-providers "./"
dotest
clean

# Test using env var 
export CONTEXT_PROVIDERS="./"
../../update-contextkit-providers/update-contextkit-providers
dotest
clean

echo "All ok!"
exit 0
