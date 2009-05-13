#!/usr/bin/python
"""provide.py -- start up a provider from the command line

SYNOPSIS

./provide [BUSTYPE:]PROVIDERNAME [TYPE NAME INITVALUE ...]

Starts up a Flexiprovider with the given PROVIDERNAME, serving
properties specified in the arguments.  TYPE is one of 'int', 'string',
'double', 'truth'.  BUSTYPE is either 'system', 'session' (or void,
defaulting to the latter).
"""

import sys
from flexiprovider import *

types = dict(int=(INT, int),
             truth=(TRUTH, bool),
             string=(STRING, str),
             double=(DOUBLE, float))
properties = []

if len(sys.argv) < 2:
    print __doc__
    sys.exit(1)
sys.argv.pop(0)
busaddress = sys.argv.pop(0).split(':')
if len(busaddress) == 1:
    busaddress.insert(0, 'session')
if busaddress[0] not in ('session', 'system'):
    sys.exit(2)
while len(sys.argv) >= 3:
    datatype, name, initvalue = sys.argv[:3]
    del sys.argv[:3]
    if datatype not in types:
        continue
    cookiecutter, conversion = types[datatype]
    properties.append(cookiecutter(name, conversion(initvalue)))

provider = Flexiprovider(properties, busaddress[1], busaddress[0])
provider.interactive()
