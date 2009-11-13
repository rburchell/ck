#!/bin/bash -e

function regendb {
    cdb -q "cache.cdb" "$1" > "$2.expected"
}

export CONTEXT_CORE_DECLARATIONS=.
../../update-contextkit-providers/update-contextkit-providers .

regendb "KEYS" "KEYS"
regendb "Battery.Charging:KEYDOC" "Battery.Charging_KEYDOC"
regendb "Battery.Charging:PROVIDERS" "Battery.Charging_PROVIDERS"
echo "Regen succeeded!"
exit 0
