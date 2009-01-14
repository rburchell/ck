#!/usr/bin/env python2.5
##
## @file mce_stub.py
##
## Copyright (C) 2008 Nokia. All rights reserved.
##
## 
## This python file export a DBUS server, which emulates the 
## objects/interfaces provided by mce daemon.
## It is used for testing contextd
##
## Requires python2.5-gobject and python2.5-dbus
##
## Implements also some testing API:
##
## com.nokia.mce
## com.nokia.mce.request
## 
##
## set_error (string error)
##
import traceback
import dbus
import sys
import dbus.service
from dbus.mainloop.glib import DBusGMainLoop
import gobject

mySubscriber = None

class Subscriber (dbus.service.Object):

    def __init__(self, main_loop, busSession):
        dbus.service.Object.__init__(self, busSession, 
                "/org/freedesktop/ContextKit/Subscriber")
        self.main_loop = main_loop
        self.subscribe = False
        
        #Hardcoded properties
        self.propStruct = { 'Context.Device.Orientation.edgeUp' : [0,1]}
        self.propArray = ["Context.Device.Orientation.facingUp"]
        
    @dbus.service.method(dbus_interface='org.freedesktop.ContextKit.Subscriber',
                       in_signature='', out_signature='a{sv}as')
    def Changed(self, propStruct, propArray):
        pass
    
    @dbus.service.method(dbus_interface='org.freedesktop.ContextKit.Subscriber',
                       in_signature='as', out_signature='a{sv}as')
    def Subscribe(self, propArray):
        if not self.subscribe:
            self.subscribe = True
            return self.propStruct, self.propArray
    
    @dbus.service.method(dbus_interface='org.freedesktop.ContextKit.Subscriber',
                       in_signature='as', out_signature='')
    def Unsuscribe(self, propArray):
        if self.subscribe:
            self.subscribe = False
        
    def isSubscribed (self):
        if self.subscribe:
            return True     
        else:
            return False

class Manager (dbus.service.Object):
    
    def __init__(self, main_loop):
        self.subscriber = None
        self.busSes = dbus.service.BusName("org.freedesktop.ContextKit", 
                                           dbus.SessionBus())
         
        dbus.service.Object.__init__(self, self.busSes, 
                "/org/freedesktop/ContextKit/Manager")
        self.main_loop = main_loop
        
        #Hardcoded properties
        self.propStruct = { 'Context.Device.Orientation.edgeUp' : [0,1]}
        self.propArray = ["Context.Device.Orientation.facingUp"]
    
    @dbus.service.method(dbus_interface='org.freedesktop.ContextKit.Manager',
                       in_signature='as', out_signature='a{sv}as')
    def Get (self, propArray):
        return self.propStruct, self.propArray
    
    @dbus.service.method(dbus_interface='org.freedesktop.ContextKit.Manager',
                       in_signature='', out_signature='o')
    def GetSubscriber(self):
        if self.subscriber == None:
            self.subscriber = Subscriber(self.main_loop, self.busSes)
            mySubscriber = self.subscriber
        return self.subscriber
    
if __name__ == "__main__":

    DBusGMainLoop(set_as_default=True)
    loop = gobject.MainLoop()
    manager_server = Manager(loop)
    loop.run()
