#!/usr/bin/env python
#!/usr/bin/env python2.5
##
## @file contextOrientationTCs.py
##
## Copyright (C) 2008 Nokia. All rights reserved.
##
## 
## 
##
## Requires python2.5-gobject and python2.5-dbus
##
## Implements also some testing API:
##
## 

import os
import sys
from time import sleep
import unittest
import traceback
import conf
import dbus
import dbus.service
from dbus.mainloop.glib import DBusGMainLoop
import gobject

class ContextDbusClient (dbus.service.Object):

    def __init__(self, main_loop):
        # Here the object path
        dbus.service.Object.__init__(self,
                 dbus.service.BusName("com.nokia.test", bus=dbus.SessionBus()),
                 "/com/nokia/test/request")

        self.main_loop = main_loop
        self.stopped = False
        self.cdBusServer = None        
    
    #
    # DBUS methods
    #

    @dbus.service.method(dbus_interface='com.nokia.test.request',
                       in_signature='', out_signature='a{sv}as')
    def get_returnedProp(self):
        return self.cdBusServer.getretProp()
    
    @dbus.service.method(dbus_interface='com.nokia.test.request',
                       in_signature='', out_signature='')
    def addSubscriber(self):
        self.cdBusServer.addSubscriber()
    
    @dbus.service.method(dbus_interface='com.nokia.test.request',
                       in_signature='', out_signature='')
    def removeSubscriber(self):
        self.cdBusServer.removeSubscriber()
        
    @dbus.service.method(dbus_interface='com.nokia.test.request',
                       in_signature='as', out_signature='')
    def setProperties(self, properties):
        self.cdBusServer.addProp(properties)
        
    @dbus.service.method(dbus_interface='com.nokia.test.request',
                       in_signature='as', out_signature='')
    def removeProperties(self, properties):
        self.cdBusServer.removeProp(properties)
        
    @dbus.service.method(dbus_interface='com.nokia.test.request',
                       in_signature='', out_signature='')
    def subscribe(self):
        self.cdBusServer.subscribe()
        
    @dbus.service.method(dbus_interface='com.nokia.test.request',
                       in_signature='', out_signature='so')
    def get_Subscriber(self):
        return self.cdBusServer.get_Subscriber()     
    
    @dbus.service.method(dbus_interface='com.nokia.test.request',
                       in_signature='o', out_signature='')
    def rm_Subscriber(self, subscriberBus):
        pass
        
    #
    # Public class methods
    #

    def setServer (self, server):
        self.cdBusServer = server

class ContextDbusServer():
    
    def __init__(self, main_loop,client):
        self.main_loop = main_loop
        self.client = client
        self.me = self
        self.client.setServer(self.me)
        self.propList = []
        self.retProp = None
        self.undetProp = None
        self.execute_subscribe()
    
    def get_Subscriber (self):
        bus = dbus.SessionBus()
        proxy_object_manager = bus.get_object("org.freedesktop.ContextKit","/org/freedesktop/ContextKit/Manager")
        iface_manager = dbus.Interface(proxy_object_manager, "org.freedesktop.ContextKit.Manager")
        #print "Requesting the subscriber object"
        # Get the subscriber object
        new_object_path = iface_manager.GetSubscriber()
        
        return new_object_path, bus
    
    def rm_Subscriber (self, bus):
        bus.close()
        
    def handle_signal(self, prop, undeterminedProp):
        print "Got signal"
        self.retProp = prop
        self.undetProp = undeterminedProp
        print self.retProp
        print undeterminedProp
        #struct1 = changed[edge_key]
        #struct2 = changed[facing_key]
        #print "Orientation:", struct1[1], struct2[1]
        #self.client.setProp(struct1)
    
    def getretProp (self):
        return self.retProp, self.undetProp
    
    def subscribe (self):
        self.iface_subscriber.Subscribe(self.propList)
    
    def unsubscribe(self, properties):
        self.iface_subscriber.Unsubscribe(properties)
    
    def addProp(self,properties):
        for prop in properties:
            self.propList.append(prop)

    def removeProp(self,properties):
        for prop in properties:
            self.propList.remove(prop)
    
    def addSubscriber(self):
        new_object_path = self.iface_manager.GetSubscriber()
        proxy_object_subscriber = bus.get_object("org.freedesktop.ContextKit", new_object_path)
        self.iface_subscriber = dbus.Interface(proxy_object_subscriber, "org.freedesktop.ContextKit.Subscriber")
        
        # Connect to the signal
        print "Connecting to the signal"
        self.iface_subscriber.connect_to_signal("Changed", self.handle_signal)
    
    def removeSubscriber(self):
        pass
    
    def execute_subscribe(self):
        bus = dbus.SessionBus()
        proxy_object_manager = bus.get_object("org.freedesktop.ContextKit","/org/freedesktop/ContextKit/Manager")
        iface_manager = dbus.Interface(proxy_object_manager, "org.freedesktop.ContextKit.Manager")
        print "Requesting the subscriber object"
        # Get the subscriber object
        new_object_path = iface_manager.GetSubscriber()
        
        print "Got an object path", new_object_path
        # And then execute the actual subscribe property_names
        
        proxy_object_subscriber = bus.get_object("org.freedesktop.ContextKit", new_object_path)
        self.iface_subscriber = dbus.Interface(proxy_object_subscriber, "org.freedesktop.ContextKit.Subscriber")
        
        # Connect to the signal
        print "Connecting to the signal"
        self.iface_subscriber.connect_to_signal("Changed", self.handle_signal)
        
        #print "Executing the Subscribe"
        #ret = iface_subscriber.Subscribe(self.propList)
        
        #print "Returned:", ret
    
def main():
    DBusGMainLoop(set_as_default=True)
    loop = gobject.MainLoop()
    manager_client = ContextDbusClient(loop)
    manager_server = ContextDbusServer(loop,manager_client)
    loop.run()
    
if __name__ == "__main__":
    main()

