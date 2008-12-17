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
import dbus_emulator
import conf


class ContextHelpers(unittest.TestCase):

    def setUp(self):
        #print os.environ['DBUS_SYSTEM_BUS_ADDRESS']
        #print os.environ['DBUS_SESSION_BUS_ADDRESS']
        
        self.sysBus = dbus.SystemBus()
        try:
            object  = self.sysBus.get_object("com.nokia.mce","/com/nokia/mce/request")
            self.ifceMCE = dbus.Interface(object,"com.nokia.mce.request")
        except dbus.DBusException:
            traceback.print_exc()
            sys.exit(1)

    def tearDown(self):
        pass
        #try:
        #    self.ifceMCE.Exit()
        #except dbus.DBusException:
        #    traceback.print_exc()
        #    sys.exit(1)

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

    def setCoord(self, x, y, z):
    
        try:
            self.ifceMCE.req_device_coord_change(x, y, z)
        except dbus.DBusException:
            traceback.print_exc()
            sys.exit(1)

    def getProp(self,properties):
        bus = dbus.SessionBus()
        try:
            object  = bus.get_object("org.freedesktop.ContextKit","/org/freedesktop/ContextKit/Manager")
            iface = dbus.Interface(object,"org.freedesktop.ContextKit.Manager")
            ret = iface.Get(properties)
        except dbus.DBusException, ex:
            raise 
        return ret
    
class DeviceOrientation(ContextHelpers):

    def test_faceup(self):
        self.setFacing("face_up")
        prop = ['Context.Device.Orientation.facingUp']
        struct = self.getProp(prop)
        self.assert_(struct['Context.Device.Orientation.facingUp'][1],True)

    def test_facedown(self):
        self.setFacing("face_down")
        prop = ['Context.Device.Orientation.facingUp']
        struct = self.getProp(prop)
        self.assert_(struct['Context.Device.Orientation.facingUp'][1],False)
        
    def test_orientationEdgeLeft(self):
        self.setFacing("face_up")
        self.setRotation("portrait")
        self.setCoord(-1000, 20, -10)
        prop = ['Context.Device.Orientation.facingUp','Context.Device.Orientation.edgeUp']
        struct = self.getProp(prop)
        self.assert_(struct['Context.Device.Orientation.facingUp'][1],True)
        self.assert_(struct['Context.Device.Orientation.edgeUp'][1],"2")
    
    def test_orientationEdgeRight(self):
        self.setFacing("face_up")
        self.setRotation("portrait")
        self.setCoord(1000, 20, -10)
        prop = ['Context.Device.Orientation.facingUp','Context.Device.Orientation.edgeUp']
        struct = self.getProp(prop)
        self.assert_(struct['Context.Device.Orientation.facingUp'][1],True)
        self.assert_(struct['Context.Device.Orientation.edgeUp'][1],"3")
        
    def test_orientationEdgeUp(self):
        self.setFacing("face_down")
        self.setCoord(20, -1000, 10)
        self.setRotation("landscape")
        prop = ['Context.Device.Orientation.facingUp','Context.Device.Orientation.edgeUp']
        struct = self.getProp(prop)
        self.assert_(struct['Context.Device.Orientation.facingUp'][1],False)
        self.assert_(struct['Context.Device.Orientation.edgeUp'][1],"1")
    
    def test_orientationEdgeDown(self):
        self.setFacing("face_down")
        self.setCoord(20, 1000, 10)
        self.setRotation("landscape")
        prop = ['Context.Device.Orientation.facingUp','Context.Device.Orientation.edgeUp']
        struct = self.getProp(prop)
        self.assert_(struct['Context.Device.Orientation.facingUp'][1],False)
        self.assert_(struct['Context.Device.Orientation.edgeUp'][1],"4")
    
    def test_inexistentProperties(self):
        prop = ['Context.Device.Something']
        self.assertRaises(dbus.DBusException,self.getProp,prop)

def testRun():
    suite = unittest.TestLoader().loadTestsFromTestCase(DeviceOrientation)
    unittest.TextTestRunner(verbosity=2).run(suite)
