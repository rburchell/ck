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

from ctypes import *
import dbus
import dbus.service
from dbus.mainloop.glib import DBusGMainLoop
import gobject

class Provider (dbus.service.Object):
    def __init__(self, main_loop):
        # Load the libcontextprovider library
        self.libc = CDLL("libcontextprovider.so.0.0.0")
        
        # Initiate dbus connection so that this stub can be commanded throug it
        self.busSes = dbus.service.BusName("org.freedesktop.ContextKit.Testing.Provider",
                                           dbus.SessionBus())
         
        dbus.service.Object.__init__(self, self.busSes, 
                "/org/freedesktop/ContextKit/Testing/Provider")
        self.main_loop = main_loop
        

    @dbus.service.method(dbus_interface='org.freedesktop.ContextKit.Testing.Provider',
                       in_signature='', out_signature='')
    def Start (self):
        print "Provider: Starting"
        key1 = c_char_p("Context.Test.keyInt")
        key2 = c_char_p("Context.Test.keyDouble")
        key3 = c_char_p("Context.Test.keyString")
        keyListType = c_char_p * 4
        keyList = keyListType(key1, key2, key3, None)
        print "Provider: initing"
        self.libc.context_provider_init(keyList, None, None, None, None, None, None)
   
    @dbus.service.method(dbus_interface='org.freedesktop.ContextKit.Testing.Provider',
                       in_signature='', out_signature='')
    def Exit (self):
        #print "Provider: Exiting"
        self.main_loop.quit()

if __name__ == "__main__":
    DBusGMainLoop(set_as_default=True)
    loop = gobject.MainLoop()
    fakeProvider = Provider(loop)
    loop.run()
    print "Provider: Returned from run"
