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
    
    def resetSignalStatus(self):
        self.receivedSignal = False
        
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
        
        self.proxy_object_subscriber = self.bus.get_object(providerBusName, 
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
        
        signalHandler = self.subscribers [subscriberId][2]
        
        signalHandler.resetSignalStatus()
        
        for prop in properties:
            if self.subscribers[subscriberId][1].__contains__(prop):
                self.subscribers [subscriberId][1].remove(prop)
        
        iface_subscriber.Unsubscribe(properties)
    
    
    @dbus.service.method(dbus_interface=cfg.scriberHandlerIfce,
                       in_signature='', out_signature='b')
    def getSignalStatus(self, subscriberId):
        return self.subscribers [subscriberId][2].getSignalStatus()
        
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
    
def main():
    DBusGMainLoop(set_as_default=True)
    loop = gobject.MainLoop()
    subHandler = SubscriptionHandler(loop)
    loop.run()
    
if __name__ == "__main__":
    main()

