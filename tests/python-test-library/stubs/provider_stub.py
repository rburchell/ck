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

    def get_cb (self, ss, cs, d):
        self.log += ("(get_cb(" + cb.StringSet.debug(ss) + "))")
        return 0


    def first_cb (self, ss, d):
        self.log += ("(first_cb(" + cb.StringSet.debug(ss) + "))")
        return 0

    def last_cb (self, ss, d):
        self.log += ("(last_cb(" + cb.StringSet.debug(ss) + "))")
        return 0

    def __init__(self, main_loop):
        self.log = "";
        # Initiate dbus connection so that this stub can be commanded throug it
        self.busSes = dbus.service.BusName("org.freedesktop.ContextKit.Testing.Provider.Command",
                                           dbus.SessionBus())

        dbus.service.Object.__init__(self, self.busSes,
                "/org/freedesktop/ContextKit/Testing/Provider")
        self.main_loop = main_loop

    @dbus.service.method(dbus_interface='org.freedesktop.ContextKit.Testing.Provider',
                       in_signature='', out_signature='')
    def DoInit(self):
        print "Provider: Executing Init"
        cb.ContextProvider.init(True, "org.freedesktop.ContextKit.Testing.Provider")

    @dbus.service.method(dbus_interface='org.freedesktop.ContextKit.Testing.Provider',
                       in_signature='', out_signature='')
    def DoInstall(self):
        print "Provider: Executing Install"
        self.provider = cb.ContextProvider.install(["test.string", "test.int"],
                         cb.ContextProvider.GET_CALLBACK(self.get_cb), None,
                         cb.ContextProvider.SUBSCRIBE_CALLBACK(self.first_cb), None,
                         cb.ContextProvider.SUBSCRIBE_CALLBACK(self.last_cb), None)

    @dbus.service.method(dbus_interface='org.freedesktop.ContextKit.Testing.Provider',
                       in_signature='', out_signature='')
    def DoRemove(self):
        print "Provider: Executing Remove"
        cb.ContextProvider.remove(self.provider)

    @dbus.service.method(dbus_interface='org.freedesktop.ContextKit.Testing.Provider',
                       in_signature='', out_signature='')
    def Exit (self):
        print "Provider: Exiting"
        self.main_loop.quit()

    @dbus.service.method(dbus_interface='org.freedesktop.ContextKit.Testing.Provider',
                       in_signature='', out_signature='')
    def ResetLog (self):
        self.log = "";

    @dbus.service.method(dbus_interface='org.freedesktop.ContextKit.Testing.Provider',
                       in_signature='', out_signature='s')
    def GetLog (self):
        return self.log;

if __name__ == "__main__":
    DBusGMainLoop(set_as_default=True)
    loop = gobject.MainLoop()

    fakeProvider = FakeProvider(loop)
    loop.run()
    print "Provider: Returned from run"
