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
    cdb -q "cache.cdb" "$1" > "$2.actual"
}

# Check the keys in the existing cache.cdb database
function dotest {
    querydb "KEYS" "KEYS"
    compare "KEYS.expected" "KEYS.actual"

    querydb "PROVIDERS" "PROVIDERS"
    compare "PROVIDERS.expected" "PROVIDERS.actual"

    querydb "org.freedesktop.ContextKit.contextd1:KEYS" "org.freedesktop.ContextKit.contextd1_KEYS"
    compare "org.freedesktop.ContextKit.contextd1_KEYS.expected" "org.freedesktop.ContextKit.contextd1_KEYS.actual"

    querydb "org.freedesktop.ContextKit.contextd2:KEYS" "org.freedesktop.ContextKit.contextd2_KEYS"
    compare "org.freedesktop.ContextKit.contextd2_KEYS.expected" "org.freedesktop.ContextKit.contextd2_KEYS.actual"

    querydb "Battery.Charging:KEYTYPE" "Battery.Charging_KEYTYPE"
    compare "Battery.Charging_KEYTYPE.expected" "Battery.Charging_KEYTYPE.actual"

    querydb "Battery.Charging:KEYDOC" "Battery.Charging_KEYDOC"
    compare "Battery.Charging_KEYDOC.expected" "Battery.Charging_KEYDOC.actual"

    querydb "Battery.Charging:KEYPROVIDER" "Battery.Charging_KEYPROVIDER"
    compare "Battery.Charging_KEYPROVIDER.expected" "Battery.Charging_KEYPROVIDER.actual"

    querydb "Battery.Charging:KEYBUS" "Battery.Charging_KEYBUS"
    compare "Battery.Charging_KEYBUS.expected" "Battery.Charging_KEYBUS.actual"
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
