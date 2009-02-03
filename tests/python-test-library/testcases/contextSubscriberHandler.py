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
import conf as cfg
import dbus
import dbus.service
from dbus.mainloop.glib import DBusGMainLoop
import gobject

class ContextDbusClient (dbus.service.Object):

    def __init__(self, main_loop):
        # Here the object path
        dbus.service.Object.__init__(self,
                 dbus.service.BusName(cfg.testBusName, bus=dbus.SessionBus()),
                 cfg.testRqstPath)

        self.main_loop = main_loop
        self.stopped = False
        self.cdBusServer = None        
    
    #
    # DBUS methods
    #

    @dbus.service.method(dbus_interface=cfg.testRqstIfce,
                       in_signature='', out_signature='a{sv}as')
    def getChangedProp(self):
        return self.cdBusServer.getChangedProp()
    
    @dbus.service.method(dbus_interface=cfg.testRqstIfce,
                       in_signature='', out_signature='s')
    def getSubscriber(self):
        return self.cdBusServer.getSubscriber()
    
    @dbus.service.method(dbus_interface=cfg.testRqstIfce,
                       in_signature='', out_signature='')
    def removeSubscriber(self):
        self.cdBusServer.removeSubscriber()
        
    @dbus.service.method(dbus_interface=cfg.testRqstIfce,
                       in_signature='as', out_signature='')
    def setProperties(self, properties):
        self.cdBusServer.addProp(properties)
        
    @dbus.service.method(dbus_interface=cfg.testRqstIfce,
                       in_signature='as', out_signature='')
    def removeProperties(self, properties):
        self.cdBusServer.removeProp(properties)
        
    @dbus.service.method(dbus_interface=cfg.testRqstIfce,
                       in_signature='as', out_signature='a{sv}as')
    def subscribe(self, properties):
        return self.cdBusServer.subscribe(properties)
        
    @dbus.service.method(dbus_interface=cfg.testRqstIfce,
                       in_signature='', out_signature='')
    def unsubscribe(self):
        self.cdBusServer.unsubscribe()
        
    @dbus.service.method(dbus_interface=cfg.testRqstIfce,
                       in_signature='', out_signature='')
    def loopQuit(self):
        print "quit"
        self.main_loop.quit()


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
        self.changedRetProp = None
        self.changedUndetProp = None
        self.bus = None
        self.subscribers = []
        #self.execute_subscribe()
    
    def getSubscriber (self):
        self.bus = dbus.SessionBus()
        proxy_object_manager = self.bus.get_object(cfg.ctxBusName,
                                        cfg.ctxMgrPath)
        iface_manager = dbus.Interface(proxy_object_manager, 
                                       cfg.ctxMgrIfce)
        subscriber_object_path = iface_manager.GetSubscriber()
        
        proxy_object_subscriber = self.bus.get_object(cfg.ctxBusName, 
                                                 subscriber_object_path)
        
        #self.subscribers.append(proxy_object_subscriber)
        
        self.iface_subscriber = dbus.Interface(proxy_object_subscriber,
                                               cfg.ctxScriberIfce)
        
        self.iface_subscriber.connect_to_signal("Changed", self.handle_signal)
        
        return str(subscriber_object_path)
    
    def subscribe (self, properties):
        returnedProp, undetermined = self.iface_subscriber.Subscribe(properties)
        return returnedProp, undetermined
    
    def unsubscribe(self, properties):
        self.iface_subscriber.Unsubscribe(properties)
        
    
    
    def rm_Subscriber (self):
        self.main_loop.quit()
        #self.bus.close()
        
    def handle_signal(self, prop, undeterminedProp):
        self.changedRetProp = prop
        self.changedUndetProp = undeterminedProp
        #print self.retProp
        #print undeterminedProp
        #struct1 = changed[edge_key]
        #struct2 = changed[facing_key]
        #print "Orientation:", struct1[1], struct2[1]
        #self.client.setProp(struct1)
    
    def getChangedProp (self):
        return self.changedRetProp, self.changedUndetProp
    
    def addProp(self,properties):
        for prop in properties:
            self.propList.append(prop)

    def removeProp(self,properties):
        for prop in properties:
            self.propList.remove(prop)
    
    def addSubscriber(self):
        new_object_path = self.iface_manager.GetSubscriber()
        proxy_object_subscriber = bus.get_object(cfg.ctxBusName, new_object_path)
        self.iface_subscriber = dbus.Interface(proxy_object_subscriber,
                                               cfg.ctxScriberIfce)
        
        # Connect to the signal
        print "Connecting to the signal"
        self.iface_subscriber.connect_to_signal("Changed", self.handle_signal)
    
    def removeSubscriber(self):
        pass
    
    def execute_subscribe(self):
        bus = dbus.SessionBus()
        proxy_object_manager = bus.get_object(cfg.ctxBusName,
                                              cfg.ctxMgrPath)
        iface_manager = dbus.Interface(proxy_object_manager, cfg.ctxMgrIfce)
        print "Requesting the subscriber object"
        # Get the subscriber object
        new_object_path = iface_manager.GetSubscriber()
        
        print "Got an object path", new_object_path
        # And then execute the actual subscribe property_names
        
        proxy_object_subscriber = bus.get_object(cfg.ctxBusName, new_object_path)
        self.iface_subscriber = dbus.Interface(proxy_object_subscriber,
                                        cfg.ctxScriberIfce)
        
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

