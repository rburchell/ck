#!/bin/bash -e

function regendb {
    cdb -q "cache.cdb" "$1" > "$2.expected"
}

export CONTEXT_CORE_DECLARATIONS=.
../../update-contextkit-providers/update-contextkit-providers .

regendb "KEYS" "KEYS"
regendb "PLUGINS" "PLUGINS"
regendb "contextkit-dbus:KEYS" "contextkit-dbus_KEYS"
regendb "Battery.Charging:KEYTYPE" "Battery.Charging_KEYTYPE"
regendb "Battery.Charging:KEYDOC" "Battery.Charging_KEYDOC"
regendb "Battery.Charging:KEYPLUGIN" "Battery.Charging_KEYPLUGIN"
regendb "Battery.Charging:KEYCONSTRUCTIONSTRING" "Battery.Charging_KEYCONSTRUCTIONSTRING"
echo "Regen succeeded!"
exit 0
