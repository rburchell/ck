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
import dbus

class ContextHelpers(unittest.TestCase):

    def setUp(self):
        os.system("./mce_stub.py &")
        sleep (5)
        self.sysBus = dbus.SystemBus()
        try:
            object  = self.sysBus.get_object("com.nokia.mce","/com/nokia/mce/request")
            self.ifceMCE = dbus.Interface(object,"com.nokia.mce.request")
        except dbus.DBusException:
            traceback.print_exc()
            sys.exit(1)
        
    def tearDown(self):
    
        try:
            self.ifceMCE.Exit()
        except dbus.DBusException:
            traceback.print_exc()
            sys.exit(1)

    def setFacing(self,facingState):
        
        try:
            self.ifceMCE.req_device_facing_change(facingState)
        except dbus.DBusException:
            traceback.print_exc()
            sys.exit(1)

    def setRotation(self,rotationState):
    
        try:
            self.ifceMCE.req_device_rotation_change(rotationState)
        except dbus.DBusException:
            traceback.print_exc()
            sys.exit(1)

    
class DeviceOrientation(ContextHelpers):

    def test_faceup(self):
        self.setFacing("face_up")
        prop = ['Context.Device.Orientation.displayFacingUp','Context.Device.Orientation.displayFacingDown']
        bus = dbus.SessionBus()
        try:
            object  = bus.get_object("org.freedesktop.ContextKit","/org/freedesktop/ContextKit/Manager")
            iface = dbus.Interface(object,"org.freedesktop.ContextKit.Manager")
            ret = iface.Get(prop)
            struct = ret['Context.Device.Orientation.displayFacingUp']
            self.assert_(struct[1],"1")
            #struct = ret['Context.Device.Orientation.displayFacingDown']
            #self.assert_(struct[1],"0")
        except dbus.DBusException:
            traceback.print_exc()
            sys.exit(1)

    def test_facedown(self):
        self.setFacing("face_down")
        prop = ['Context.Device.Orientation.displayFacingUp','Context.Device.Orientation.displayFacingDown']
        bus = dbus.SessionBus()
        try:
            object  = bus.get_object("org.freedesktop.ContextKit","/org/freedesktop/ContextKit/Manager")
            iface = dbus.Interface(object,"org.freedesktop.ContextKit.Manager")
            ret = iface.Get(prop)
            struct = ret['Context.Device.Orientation.displayFacingDown']
            self.assert_(struct[1],"1")
            #struct = ret['Context.Device.Orientation.displayFacingDown']
            #self.assert_(struct[1],"0")
        except dbus.DBusException:
            traceback.print_exc()
            sys.exit(1)

    def test_orientationPortrait(self):
        self.setRotation("portrait")
        prop = ['Context.Device.Orientation.inLandscape','Context.Device.Orientation.inPortrait']
        bus = dbus.SessionBus()
        try:
            object  = bus.get_object("org.freedesktop.ContextKit","/org/freedesktop/ContextKit/Manager")
            iface = dbus.Interface(object,"org.freedesktop.ContextKit.Manager")
            ret = iface.Get(prop)
            struct = ret['Context.Device.Orientation.inPortrait']
            self.assert_(struct[1],"1")
            #struct = ret['Context.Device.Orientation.displayFacingDown']
            #self.assert_(struct[1],"0")
        except dbus.DBusException:
            traceback.print_exc()
            sys.exit(1)

    def test_orientationLandscape(self):
        self.setRotation("landscape")
        prop = ['Context.Device.Orientation.inLandscape','Context.Device.Orientation.inPortrait']
        bus = dbus.SessionBus()
        try:
            object  = bus.get_object("org.freedesktop.ContextKit","/org/freedesktop/ContextKit/Manager")
            iface = dbus.Interface(object,"org.freedesktop.ContextKit.Manager")
            ret = iface.Get(prop)
            struct1 = ret['Context.Device.Orientation.inLandscape']
            self.assert_(struct1[1],"1")

        except dbus.DBusException:
            traceback.print_exc()
            sys.exit(1)

suite = unittest.TestLoader().loadTestsFromTestCase(DeviceOrientation)
unittest.TextTestRunner(verbosity=2).run(suite)
