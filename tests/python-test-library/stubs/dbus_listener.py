#!/usr/bin/env python2.5
##
## @file dbus_listener.py
##
## Copyright (C) 2008 Nokia. All rights reserved.
##
##
## This python file is used for testing: it listens
## to NameOwnerChanged signals over DBus.
##
## Requires python2.5-gobject and python2.5-dbus
##
##

import dbus
import dbus.service
from dbus.mainloop.glib import DBusGMainLoop
import gobject

class Listener (dbus.service.Object):
    def __init__(self, main_loop):
        print "init"
        # Initiate dbus connection so that this stub can be commanded throug it
        self.busSes = dbus.service.BusName("org.freedesktop.ContextKit.Testing.Listener",
                                           dbus.SessionBus())

        dbus.service.Object.__init__(self, self.busSes,
                "/org/freedesktop/ContextKit/Testing/Listener")

        self.main_loop = main_loop

        # Listen to NameOwnerChanged
        self.bus = dbus.SessionBus()
        self.bus.add_signal_receiver(self.onNameOwnerChanged, 'NameOwnerChanged', None, None)

        # Logging
        self.nameToCheck = ""
        self.log = ""

    @dbus.service.method(dbus_interface='org.freedesktop.ContextKit.Testing.Listener',
                       in_signature='s', out_signature='')
    def SetTarget(self, targetName):
        print "Listener: SetTarget", targetName
        self.nameToCheck = targetName

    def onNameOwnerChanged(self, name, old, new):
        #print "Listener: NameOwnerChanged", name, old, new
        if name == self.nameToCheck:
            if old == "":
                self.log += ("(TargetAppeared)")
            else:
                if new == "":
                    self.log += ("(TargetDisappeared)")

        #print "Listener: log is", self.log

    @dbus.service.method(dbus_interface='org.freedesktop.ContextKit.Testing.Listener',
                       in_signature='', out_signature='s')
    def GetLog(self):
        return self.log

    @dbus.service.method(dbus_interface='org.freedesktop.ContextKit.Testing.Listener',
                       in_signature='', out_signature='')
    def Exit(self):
        #print "Listener: Exiting"
        self.main_loop.quit()


if __name__ == "__main__":
    DBusGMainLoop(set_as_default=True)
    loop = gobject.MainLoop()
    listener = Listener(loop)
    loop.run()
    print "Listener: Returned from run"
