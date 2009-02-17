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
sys.path.append("./python/") # libcontextprovider python bindings
import ContextProvider as cp

sys.path.append("./tests/python-test-library/testcases/")
import conf as cfg

import dbus
import dbus.service
from dbus.mainloop.glib import DBusGMainLoop
import gobject

class FakeProvider (dbus.service.Object):

    # Callback functions for libcontextd
    def py_get_cb (self, ss, cs, d):
        self.log += ("(py_get_cb(" + cp.StringSet.debug(ss) + "))")
        # Set test.int to 5 and test.string to undetermined
        cp.ContextProvider.change_set_add_integer(cs, "test.int", 5)
        cp.ContextProvider.change_set_add_undetermined_key(cs, "test.double")


    def py_subscribed_cb (self, ss, d):
        self.log += ("(py_subscribed_cb(" + cp.StringSet.debug(ss) + "))")

    def py_unsubscribed_cb (self, keys, keys_remaining, d):
        self.log += ("(py_unsubscribed_cb(" + cp.StringSet.debug(keys) + ", " + cp.StringSet.debug(keys_remaining) + "))")

    # Initializing the provider object
    def __init__(self, main_loop):
        self.log = "";
        # Initiate dbus connection so that this stub can be commanded throug it
        self.busSes = dbus.service.BusName(cfg.fakeProviderBusName,
                                           dbus.SessionBus())

        dbus.service.Object.__init__(self, self.busSes,
                cfg.fakeProviderPath)
        self.main_loop = main_loop

    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='b', out_signature='')
    def DoInit(self, useSession):
        print "Provider: Executing Init"
        if useSession:
            print "Provider: Using session bus"
            cp.ContextProvider.init(0, cfg.fakeProviderLibBusName)
        else:
            print "Provider: Using system bus"
            cp.ContextProvider.init(1, cfg.fakeProviderLibBusName)
        print "Provider: Init done"


    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='', out_signature='')
    def DoInstall(self):
        print "Provider: Executing Install"
        self.get_cb = cp.ContextProvider.GET_CALLBACK(self.py_get_cb)
        self.subscribed_cb = cp.ContextProvider.SUBSCRIBED_CALLBACK(self.py_subscribed_cb)
        self.unsubscribed_cb = cp.ContextProvider.UNSUBSCRIBED_CALLBACK(self.py_unsubscribed_cb)


        self.provider = cp.ContextProvider.install(["test.double", "test.int", "test.bool"],
                         self.get_cb, None,
                         self.subscribed_cb, None,
                         self.unsubscribed_cb, None)

    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='', out_signature='')
    def DoRemove(self):
        print "Provider: Executing Remove"
        cp.ContextProvider.remove(self.provider)

    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='a{sv}as', out_signature='')
    def SendChangeSet(self, values, undetermined):
        cs = cp.ContextProvider.change_set_create()

        for key in values.keys():
            value = values[key]
            if type(value) is dbus.Int32:
                cp.ContextProvider.change_set_add_integer(cs, key, value)
            elif type(value) is dbus.Double:
                cp.ContextProvider.change_set_add_double(cs, key, value)
            elif type(value) is dbus.Boolean:
                cp.ContextProvider.change_set_add_boolean(cs, key, value)
            # Note: add more types here if needed.

        if len(undetermined) > 0:
            for key in undetermined:
                cp.ContextProvider.change_set_add_undetermined_key(cs, str(key))

        cp.ContextProvider.change_set_commit(cs)

    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='', out_signature='')
    def CancelChangeSet(self):
        cs = cp.ContextProvider.change_set_create()
        cp.ContextProvider.change_set_add_double(cs, "test.double", 2.13)
        cp.ContextProvider.change_set_add_undetermined_key(cs, "test.bool")
        cp.ContextProvider.change_set_cancel(cs)

    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='s', out_signature='i')
    def GetSubscriberCount(self, key):
        return cp.ContextProvider.no_of_subscribers(key)

    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='', out_signature='')
    def Exit (self):
        print "Provider: Exiting"
        self.main_loop.quit()

    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='', out_signature='')
    def ResetLog (self):
        self.log = "";

    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='', out_signature='s')
    def GetLog (self):
        return self.log;

if __name__ == "__main__":
    DBusGMainLoop(set_as_default=True)
    loop = gobject.MainLoop()

    fakeProvider = FakeProvider(loop)
    loop.run()
    print "Provider: Returned from run"
