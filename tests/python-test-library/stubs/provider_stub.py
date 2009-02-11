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
import ContextProvider as cb

sys.path.append("./tests/python-test-library/testcases/")
import conf as cfg

import dbus
import dbus.service
from dbus.mainloop.glib import DBusGMainLoop
import gobject


class FakeProvider (dbus.service.Object):

    # Callback functions for libcontextd
    def py_get_cb (self, ss, cs, d):
        self.log += ("(py_get_cb(" + cb.StringSet.debug(ss) + "))")
        # Set test.int to 5 and test.string to undetermined
        cb.ContextProvider.change_set_add_integer(cs, "test.int", 5)
        cb.ContextProvider.change_set_add_undetermined_key(cs, "test.double")
        return 0


    def py_subscribed_cb (self, ss, d):
        self.log += ("(py_subscribed_cb(" + cb.StringSet.debug(ss) + "))")
        return 0

    def py_unsubscribed_cb (self, keys, keys_remaining, d):
        self.log += ("(py_unsubscribed_cb(" + cb.StringSet.debug(keys) + ", " + cb.StringSet.debug(keys_remaining) + "))")
        return 0

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
            cb.ContextProvider.init(0, cfg.fakeProviderLibBusName)
        else:
            print "Provider: Using system bus"
            cb.ContextProvider.init(1, cfg.fakeProviderLibBusName)
        print "Provider: Init done"


    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='', out_signature='')
    def DoInstall(self):
        print "Provider: Executing Install"
        self.get_cb = cb.ContextProvider.GET_CALLBACK(self.py_get_cb)
        self.subscribed_cb = cb.ContextProvider.SUBSCRIBED_CALLBACK(self.py_subscribed_cb)
        self.unsubscribed_cb = cb.ContextProvider.UNSUBSCRIBED_CALLBACK(self.py_unsubscribed_cb)


        self.provider = cb.ContextProvider.install(["test.double", "test.int", "test.bool"],
                         self.get_cb, None,
                         self.subscribed_cb, None,
                         self.unsubscribed_cb, None)

    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='', out_signature='')
    def DoRemove(self):
        print "Provider: Executing Remove"
        cb.ContextProvider.remove(self.provider)

    # Hard-coded change sets for different tests.
    # Could be refactored so that change sets are given as parameter,
    # if more tests are needed.
    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='', out_signature='')
    def SendChangeSet1(self):
        cs = cb.ContextProvider.change_set_create()
        cb.ContextProvider.change_set_add_double(cs, "test.double", 2.13)
        cb.ContextProvider.change_set_add_undetermined_key(cs, "test.bool")
        cb.ContextProvider.change_set_commit(cs)

    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='', out_signature='')
    def SendChangeSet2(self):
        cs = cb.ContextProvider.change_set_create()
        cb.ContextProvider.change_set_add_integer(cs, "test.int", 1)
        cb.ContextProvider.change_set_add_double(cs, "test.double", 3.1415)
        cb.ContextProvider.change_set_add_boolean(cs, "test.bool", False)
        cb.ContextProvider.change_set_commit(cs)

    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='', out_signature='')
    def SendChangeSetWithAllDataTypes(self):
        cs = cb.ContextProvider.change_set_create()
        cb.ContextProvider.change_set_add_integer(cs, "test.int", -8)
        cb.ContextProvider.change_set_add_double(cs, "test.double", 0.2)
        cb.ContextProvider.change_set_add_boolean(cs, "test.bool", True)
        # FIXME: Add new data types here as needed
        cb.ContextProvider.change_set_commit(cs)

    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='', out_signature='')
    def SendChangeSetWithAllUndetermined(self):
        cs = cb.ContextProvider.change_set_create()
        cb.ContextProvider.change_set_add_undetermined_key(cs, "test.int")
        cb.ContextProvider.change_set_add_undetermined_key(cs, "test.double")
        cb.ContextProvider.change_set_add_undetermined_key(cs, "test.bool")
        cb.ContextProvider.change_set_commit(cs)

    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='', out_signature='')
    def CancelChangeSet(self):
        cs = cb.ContextProvider.change_set_create()
        cb.ContextProvider.change_set_add_double(cs, "test.double", 2.13)
        cb.ContextProvider.change_set_add_undetermined_key(cs, "test.bool")
        cb.ContextProvider.change_set_cancel(cs)

    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='s', out_signature='i')
    def GetSubscriberCount(self, key):
        return cb.ContextProvider.no_of_subscribers(key)

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
