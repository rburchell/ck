#!/usr/bin/python
"""
A very simple example of the Flexiprovider.  Exports three properties
initially.
"""

from flexiprovider import *

Flexiprovider([INT('location.altitude'),
               STRING('my.name', 'flexi'),
               TRUTH('is.out.there')],
              'ctx.flexi.provider', 'session').interactive()
