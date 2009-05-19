#!/bin/bash

function clean {
    `rm -f *.actual`
    `rm -f *.cdb`
}

function compare {
    f=`diff -u "$1" "$2"`
    if [ $? != 0 ] ; then
        echo "$1 and $2 differ!"
        echo "$f"
        clean
        exit 128
    fi
}

BASEDIR=`dirname $0`
../../update-context-providers/update-context-providers "./"

cdb -q "context-providers.cdb" "KEYS" > "KEYS.actual"
compare "KEYS.expected" "KEYS.actual"

cdb -q "context-providers.cdb" "PROVIDERS" > "PROVIDERS.actual"
compare "PROVIDERS.expected" "PROVIDERS.actual"

cdb -q "context-providers.cdb" "org.freedesktop.ContextKit.contextd1:KEYS" > "org.freedesktop.ContextKit.contextd1:KEYS.actual"
compare "org.freedesktop.ContextKit.contextd1:KEYS.expected" "org.freedesktop.ContextKit.contextd1:KEYS.actual"

cdb -q "context-providers.cdb" "org.freedesktop.ContextKit.contextd2:KEYS" > "org.freedesktop.ContextKit.contextd2:KEYS.actual"
compare "org.freedesktop.ContextKit.contextd2:KEYS.expected" "org.freedesktop.ContextKit.contextd2:KEYS.actual"

cdb -q "context-providers.cdb" "Battery.Charging:KEYTYPE" > "Battery.Charging:KEYTYPE.actual"
compare "Battery.Charging:KEYTYPE.expected" "Battery.Charging:KEYTYPE.actual"

cdb -q "context-providers.cdb" "Battery.Charging:KEYDOC" > "Battery.Charging:KEYDOC.actual"
compare "Battery.Charging:KEYDOC.expected" "Battery.Charging:KEYDOC.actual"

cdb -q "context-providers.cdb" "Battery.Charging:KEYPROVIDER" > "Battery.Charging:KEYPROVIDER.actual"
compare "Battery.Charging:KEYPROVIDER.expected" "Battery.Charging:KEYPROVIDER.actual"

cdb -q "context-providers.cdb" "Battery.Charging:KEYBUS" > "Battery.Charging:KEYBUS.actual"
compare "Battery.Charging:KEYBUS.expected" "Battery.Charging:KEYBUS.actual"

echo "All ok!"
clean
exit 0
