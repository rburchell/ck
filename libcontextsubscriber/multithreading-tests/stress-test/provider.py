#!/usr/bin/python
"""A test provider for the stress testing."""


# change registry this often [msec]
registryChangeTimeout = 2017


from ContextKit.flexiprovider import *
import gobject
import time
import os

def update():
    t = time.time()
    dt = int(1000*(t - round(t)))
    gobject.timeout_add(1000 - dt, update)
    v = int(round(t))
    fp.set('test.int', v)
    fp.set('test.int2', v)
    print t
    return False

pcnt = 0
def chgRegistry():
    global pcnt
    pcnt += 1
    if pcnt % 2:
        print "1 provider"
        os.system('cp 1provider.cdb  cache.cdb')
    else:
        print "2 providers"
        os.system('cp 2providers.cdb cache.cdb')
    return True


gobject.timeout_add(1000, update)
# uncoment this to see the "Bus error" XXX
gobject.timeout_add(registryChangeTimeout, chgRegistry)

fp = Flexiprovider([INT('test.int'), INT('test.int2')], 'my.test.provider', 'session')
fp.run()
