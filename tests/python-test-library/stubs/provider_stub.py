#!/usr/bin/env python2.5
##
## @file provider_stub.py
##
## Copyright (C) 2008 Nokia. All rights reserved.
##
##
## This python file emulates a provider which uses
## libcontextprovider to provide context properties.
##
## Requires python2.5-gobject and python2.5-dbus
##
##

import sys
sys.path.append("./python/")
import ContextProvider as cb

import dbus
import dbus.service
from dbus.mainloop.glib import DBusGMainLoop
import gobject

class FakeProvider (dbus.service.Object):

    def get_cb (ss, cs, d):
        print StringSet.debug(ss)

    def first_cb (ss, d):
        print StringSet.debug(ss)

    def last_cb (ss, d):
        print StringSet.debug(ss)

    def __init__(self, main_loop):
        # Initiate dbus connection so that this stub can be commanded throug it
        self.busSes = dbus.service.BusName("org.freedesktop.ContextKit.Testing.Provider.Command",
                                           dbus.SessionBus())

        dbus.service.Object.__init__(self, self.busSes,
                "/org/freedesktop/ContextKit/Testing/Provider")
        self.main_loop = main_loop

    @dbus.service.method(dbus_interface='org.freedesktop.ContextKit.Testing.Provider',
                       in_signature='', out_signature='')
    def DoInit (self):
        print "Provider: Executing Init"
        cb.ContextProvider.init(True, "org.freedesktop.ContextKit.Testing.Provider")

    @dbus.service.method(dbus_interface='org.freedesktop.ContextKit.Testing.Provider',
                       in_signature='', out_signature='')
    def Exit (self):
        print "Provider: Exiting"
        self.main_loop.quit()

if __name__ == "__main__":
    DBusGMainLoop(set_as_default=True)
    loop = gobject.MainLoop()

    fakeProvider = FakeProvider(loop)
    loop.run()
    print "Provider: Returned from run"
