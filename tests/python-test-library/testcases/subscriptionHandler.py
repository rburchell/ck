#!/usr/bin/env python
#!/usr/bin/env python2.5
##
## @file subscriptionHandler.py
##
## Copyright (C) 2008 Nokia. All rights reserved.
##
## 
## 
##
## Requires python2.5-gobject and python2.5-dbus
##
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

class ChangedSignalHandler ():
    
    def __init__(self):
        self.receivedSignal = False
        self.changedProp = None
        self.changedUndetProp = None
        
    def handleSignal(self, propList, unknownList):
        self.receivedSignal = True
        self.changedProp = propList
        self.changedUndetProp = unknownList
    
    def getChangedProp(self):
        return self.changedProp
    
    def getChangedUnknownProp(self):
        return self.changedUndetProp
    
    def getSignalStatus(self):
        return self.receivedSignal
        
class SubscriptionHandler (dbus.service.Object):

    def __init__(self, main_loop):
        
        #Export on session bus Subscription Handler on org.freedesktop.context.testing
        dbus.service.Object.__init__(self,
                 dbus.service.BusName(cfg.scriberBusName, bus=dbus.SessionBus()),
                 cfg.scriberHandlerPath)

        self.main_loop = main_loop
        self.stopped = False
        self.subscribers = {}
    
    #
    # DBUS methods
    #
    @dbus.service.method(dbus_interface=cfg.scriberHandlerIfce,
                       in_signature='bs', out_signature='s')
    
    def addSubscriber(self, sessionBus, providerBusName):
        
        if sessionBus: 
            self.bus = dbus.SessionBus()
        else:
            self.bus = dbus.SystemBus()
        
        self.proxy_object_manager = self.bus.get_object(providerBusName,
                                        cfg.ctxMgrPath)
        
        self.iface_manager = dbus.Interface(self.proxy_object_manager, 
                                       cfg.ctxMgrIfce)
        
        self.subscriber_object_path = self.iface_manager.GetSubscriber()
        
        self.proxy_object_subscriber = self.bus.get_object(cfg.ctxBusName, 
                                                 self.subscriber_object_path)
        
        iface_subscriber = dbus.Interface(self.proxy_object_subscriber,
                                               cfg.ctxScriberIfce)
        
        signalHandler = ChangedSignalHandler()
        
        iface_subscriber.connect_to_signal("Changed", signalHandler.handleSignal)
        
        self.subscribers [self.subscriber_object_path] = iface_subscriber, [""], signalHandler
        
        return self.subscriber_object_path
    
    @dbus.service.method(dbus_interface=cfg.scriberHandlerIfce,
                       in_signature='ass', out_signature='a{sv}as')
    def subscribe(self, properties, subscriberId):
        iface_subscriber = self.subscribers [subscriberId][0]
        
        for prop in properties:
            if not self.subscribers[subscriberId][1].__contains__(prop):
                self.subscribers [subscriberId][1].append(prop)
        
        return iface_subscriber.Subscribe(properties)
    
    @dbus.service.method(dbus_interface=cfg.scriberHandlerIfce,
                       in_signature='ass', out_signature='')
    def unSubscribe(self, properties, subscriberId):
        iface_subscriber = self.subscribers [subscriberId][0]
        
        for prop in properties:
            if self.subscribers[subscriberId][1].__contains__(prop):
                self.subscribers [subscriberId][1].remove(prop)
        
        iface_subscriber.Unsubscribe(properties)
    
    @dbus.service.method(dbus_interface=cfg.scriberHandlerIfce,
                       in_signature='s', out_signature='a{sv}as')
    def getChangedProp(self, subscriberId):
        signalHandler = self.subscribers [subscriberId][2]
        sleep(1)
        return signalHandler.getChangedProp(), signalHandler.getChangedUnknownProp()
    
    @dbus.service.method(dbus_interface=cfg.scriberHandlerIfce,
                       in_signature='s', out_signature='as')
    def getSubscribedProperties(self, subscriberId):
        return self.subscribers [subscriberId][1]
    
    @dbus.service.method(dbus_interface=cfg.scriberHandlerIfce,
                       in_signature='', out_signature='')
    def loopQuit(self):
        print "quit"
        self.main_loop.quit()


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
    subHandler = SubscriptionHandler(loop)
    #manager_server = ContextDbusServer(loop,manager_client)
    loop.run()
    
if __name__ == "__main__":
    main()

