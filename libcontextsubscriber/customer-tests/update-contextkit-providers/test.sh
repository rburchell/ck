#!/bin/sh -e

export CONTEXT_CORE_DECLARATIONS=/dev/null
export CONTEXT_PROVIDERS=./

# Remove all temp files
clean() {
    `rm -f *.actual`
    `rm -f *.cdb`
}

# compare file $1 to $2 and display diff if different
compare() {
    f=`cmp "$1" "$2"`
    if [ $? != 0 ] ; then
        echo "$1 and $2 differ!"
        echo "$f"
        clean
        exit 128
    fi
}

# Query the database $1 using the cdb tool and echo results to $1.actual
querydb() {
    cdb -q "cache.cdb" "$1" > "$2.actual"
}

# Check the keys in the existing cache.cdb database
dotest() {
    querydb "KEYS" "KEYS"
    compare "KEYS.expected" "KEYS.actual"

    querydb "Battery.Charging:KEYDOC" "Battery.Charging_KEYDOC"
    compare "Battery.Charging_KEYDOC.expected" "Battery.Charging_KEYDOC.actual"

    querydb "Battery.Charging:PROVIDERS" "Battery.Charging_PROVIDERS"
    compare "Battery.Charging_PROVIDERS.expected" "Battery.Charging_PROVIDERS.actual"
}

# Ensure that the cache file permissions are ok
checkperm() {
    if [ "-rw-r--r--" != "`ls -l cache.cdb | cut -c 1-10`" ] ; then
        echo "Permissions of cache.db are not 0644!"
        ls -l cache.cdb
        clean
        exit 128
    fi
}

BASEDIR=`dirname $0`
PATH=../../update-contextkit-providers/.libs/:$PATH

# Test using command line param
export CONTEXT_PROVIDERS="/tmp/wrong/path"
update-contextkit-providers "./"
dotest
checkperm
clean

# Test using env var 
export CONTEXT_PROVIDERS="./"
update-contextkit-providers
dotest
checkperm
clean

echo "All ok!"
exit 0
