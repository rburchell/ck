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
import os

# libcontextprovider python bindings
if "srcdir" in os.environ:
    # For vpath builds
    sys.path.append(os.environ["srcdir"] + "/../../python/")
else:
    sys.path.append("../../python/")

import ContextKit.ContextProvider as cp

import conf as cfg

import dbus
import dbus.service
from dbus.mainloop.glib import DBusGMainLoop
import gobject

class FakeProvider (dbus.service.Object):

    # Callback functions for libcontextd
    def py_subscription_changed_cb (self, subscribe, d):
        if subscribe:
            cp.ContextProvider.set_string("test.log", "Subscribed")
        else:
            cp.ContextProvider.set_string("test.log", "Unsubscribed")
                    
    def py_dummy_cb (self, subscribe, d):
        pass
    
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
        print "Provider: Executing Installation for a group"
        self.subscription_changed_cb = cp.ContextProvider.SUBSCRIPTION_CHANGED_CALLBACK(self.py_subscription_changed_cb)
        self.dummy_cb = cp.ContextProvider.SUBSCRIPTION_CHANGED_CALLBACK(self.py_dummy_cb)
        
        p = cp.ContextProvider.install_group(["test.double", "test.int", "test.bool",
                                 "test.string"], 1, self.subscription_changed_cb, None)
        p_other = cp.ContextProvider.install_group(["test.log"], 1, self.dummy_cb, None)
    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='', out_signature='')
    def DoInstallWithContextPrefix(self):
        self.dummy_cb = cp.ContextProvider.SUBSCRIPTION_CHANGED_CALLBACK(self.py_dummy_cb)

        p = cp.ContextProvider.install_group(["Context.test2.double", "Context.test2.int", "Context.test2.bool", "Context.test2.unknown"], 1, self.dummy_cb, None)
        p_other = cp.ContextProvider.install_key("Context.test2.string", 1, self.dummy_cb, None)
    
    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='', out_signature='')
    def DoInstallKey(self):
        print "Provider: Executing Installation for key"
        self.subscription_changed_cb = cp.ContextProvider.SUBSCRIPTION_CHANGED_CALLBACK(self.py_subscription_changed_cb)
        p = cp.ContextProvider.install_key("test.single.int", 1, self.subscription_changed_cb, None)
            
    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
                       in_signature='a{sv}as', out_signature='')
    def SendChangeSet(self, values, undetermined):
        for key in values.keys():
            value = values[key]
            if type(value) is dbus.Int32:
                cp.ContextProvider.set_integer(key, value)
            elif type(value) is dbus.Double:
                cp.ContextProvider.set_double(key, value)
            elif type(value) is dbus.Boolean:
                cp.ContextProvider.set_boolean(key, value)
            elif type(value) is dbus.String:
                cp.ContextProvider.set_string(key, value)
            # Note: add more types here if needed.

        if len(undetermined) > 0:
            for key in undetermined:
                cp.ContextProvider.set_null(str(key))

    @dbus.service.method(dbus_interface=cfg.fakeProviderIfce,
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
