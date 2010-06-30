#!/usr/bin/env python2.5
"""
A very simple example of the Flexiprovider.  Exports three properties
initially.
"""

from ContextKit.flexiprovider import *

Flexiprovider([INT('location.altitude'),
               STRING('my.name', 'flexi'),
               TRUTH('is.out.there')],
              'ctx.flexi.provider', 'session').interactive()
