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

    querydb "PLUGINS" "PLUGINS"
    compare "PLUGINS.expected" "PLUGINS.actual"

    querydb "contextkit-dbus:KEYS" "contextkit-dbus_KEYS"
    compare "contextkit-dbus_KEYS.expected" "contextkit-dbus_KEYS.actual"

    querydb "Battery.Charging:KEYTYPE" "Battery.Charging_KEYTYPE"
    compare "Battery.Charging_KEYTYPE.expected" "Battery.Charging_KEYTYPE.actual"

    querydb "Battery.Charging:KEYDOC" "Battery.Charging_KEYDOC"
    compare "Battery.Charging_KEYDOC.expected" "Battery.Charging_KEYDOC.actual"

    querydb "Battery.Charging:KEYPLUGIN" "Battery.Charging_KEYPLUGIN"
    compare "Battery.Charging_KEYPLUGIN.expected" "Battery.Charging_KEYPLUGIN.actual"

    querydb "Battery.Charging:KEYCONSTRUCTIONSTRING" "Battery.Charging_KEYCONSTRUCTIONSTRING"
    compare "Battery.Charging_KEYCONSTRUCTIONSTRING.expected" "Battery.Charging_KEYCONSTRUCTIONSTRING.actual"
}

# Ensure that the cache file permissions are ok
function checkperm {
    perms=`stat -c %a cache.cdb`
    if [ "$perms" != "644" ] ; then
        echo "Permissions of cache.db are not 644!"
        clean
        exit 128
    fi
}

BASEDIR=`dirname $0`

# Test using command line param
../../update-contextkit-providers/update-contextkit-providers "./"
dotest
checkperm
clean

# Test using env var 
export CONTEXT_PROVIDERS="./"
../../update-contextkit-providers/update-contextkit-providers
dotest
checkperm
clean

echo "All ok!"
exit 0
