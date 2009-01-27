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
    
    def getProp(self,properties):
        bus = dbus.SessionBus()
        try:
            object  = bus.get_object("org.freedesktop.ContextKit","/org/freedesktop/ContextKit/Manager")
            iface = dbus.Interface(object,"org.freedesktop.ContextKit.Manager")
            returnedProp, underterminedProp = iface.Get(properties)
        except dbus.DBusException, ex:
            raise 
        return returnedProp, underterminedProp
    
class Device(ContextHelpers):

    def test_faceup(self):
        """Device orientation to face up"""
        self.ifceMCE.req_device_facing_change("face_up")
        prop = ['Context.Device.Orientation.facingUp']
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assert_(returnedProp['Context.Device.Orientation.facingUp'],)

    def test_facedown(self):
        """Device orientation to face down"""
        self.ifceMCE.req_device_facing_change("face_down")
        prop = ['Context.Device.Orientation.facingUp']
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assert_(returnedProp['Context.Device.Orientation.facingUp'],2)
        
    def test_orientationEdgeLeft(self):
        """Device orientation to edge left"""
        self.ifceMCE.req_device_facing_change("face_up")
        self.ifceMCE.req_device_rotation_change("portrait")
        self.ifceMCE.req_device_coord_change(-1000, 20, -10)
        prop = ['Context.Device.Orientation.facingUp','Context.Device.Orientation.edgeUp']
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assert_(returnedProp['Context.Device.Orientation.facingUp'],1)
        self.assert_(returnedProp['Context.Device.Orientation.edgeUp'],"2")
    
    def test_orientationEdgeRight(self):
        """Device orientation to edge right"""
        self.ifceMCE.req_device_facing_change("face_up")
        self.ifceMCE.req_device_rotation_change("portrait")
        self.ifceMCE.req_device_coord_change(1000, 20, -10)
        prop = ['Context.Device.Orientation.facingUp','Context.Device.Orientation.edgeUp']
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assert_(returnedProp['Context.Device.Orientation.facingUp'],1)
        self.assert_(returnedProp['Context.Device.Orientation.edgeUp'],"3")
        
    def test_orientationEdgeUp(self):
        """Device orientation to edge up"""
        self.ifceMCE.req_device_facing_change("face_down")
        self.ifceMCE.req_device_rotation_change("landscape")
        self.ifceMCE.req_device_coord_change(20, -1000, 10)
        prop = ['Context.Device.Orientation.facingUp','Context.Device.Orientation.edgeUp']
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assert_(returnedProp['Context.Device.Orientation.facingUp'],2)
        self.assert_(returnedProp['Context.Device.Orientation.edgeUp'],"1")
    
    def test_orientationEdgeDown(self):
        """Device orientation to edge down"""
        self.ifceMCE.req_device_facing_change("face_down")
        self.ifceMCE.req_device_rotation_change("landscape")
        self.ifceMCE.req_device_coord_change(20, 1000, 10)
        prop = ['Context.Device.Orientation.facingUp','Context.Device.Orientation.edgeUp']
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assert_(returnedProp['Context.Device.Orientation.facingUp'],2)
        self.assert_(returnedProp['Context.Device.Orientation.edgeUp'],"4")
    
    def test_orientationEdgeFaceUndefined(self):
        """Device orientation to undefined edge and facing"""
        self.ifceMCE.req_device_facing_change("unknown")
        self.ifceMCE.req_device_rotation_change("unknown")
        self.ifceMCE.req_device_coord_change(0, 0, 0)
        prop = ['Context.Device.Orientation.facingUp','Context.Device.Orientation.edgeUp']
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assert_(returnedProp['Context.Device.Orientation.facingUp'],0)
        self.assert_(returnedProp['Context.Device.Orientation.edgeUp'],"0")
        
    def test_displayStateOn(self):
        """Device display active"""
        prop = ['Context.Device.Display.displayState']
        self.ifceMCE.req_display_state_on()
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assert_(returnedProp['Context.Device.Display.displayState'],1)
        
    def test_displayStateOff(self):
        """Device display inactive"""
        prop = ['Context.Device.Display.displayState']
        self.ifceMCE.req_display_state_off()
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assert_(returnedProp['Context.Device.Display.displayState'],0)
        
    def test_displayStateDim(self):
        """Device display dim"""
        prop = ['Context.Device.Display.displayState']
        self.ifceMCE.req_display_state_dim()
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assert_(returnedProp['Context.Device.Display.displayState'],2)
        
    def test_inActiveUseOff(self):
        """Device disactive"""
        prop = ['Context.Device.Usage.inActiveUse']
        self.ifceMCE.req_inactivity_status_active()
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assert_(returnedProp['Context.Device.Usage.inActiveUse'],True)
        
    def test_inActiveUseOn(self):
        """Device active"""
        prop = ['Context.Device.Usage.inActiveUse']
        self.ifceMCE.req_inactivity_status_inactive()
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assert_(returnedProp['Context.Device.Usage.inActiveUse'],False)
                

class Environment(ContextHelpers):
    def test_flightModeOn(self):
        """Device mode to flight"""
        prop = ['Context.Environment.Connection.CurrentData.isFlightMode']
        self.ifceMCE.req_device_mode_change("flight")
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assert_(returnedProp['Context.Environment.Connection.CurrentData.isFlightMode'],True)
    
    def test_flightModeOff(self):
        """Device mode disable flight mode"""
        prop = ['Context.Environment.Connection.CurrentData.isFlightMode']
        self.ifceMCE.req_device_mode_change("normal")
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assert_(returnedProp['Context.Environment.Connection.CurrentData.isFlightMode'],False)
    
    def test_emergencyModeNoSim(self):
        """Device emergency mode without SIM"""
        prop = ['Context.Environment.Connection.CurrentData.isEmergencyMode']
        self.ifceMCE.req_call_state_change("none","emergency")
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assert_(returnedProp['Context.Environment.Connection.CurrentData.isEmergencyMode'],False)
                     
    def test_emergencyModeSim(self):
        """Device emergency mode with SIM"""
        prop = ['Context.Environment.Connection.CurrentData.isEmergencyMode']
        self.ifceMCE.req_call_state_change("cellular","emergency")
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assert_(returnedProp['Context.Environment.Connection.CurrentData.isEmergencyMode'],True)

    def test_normalModeNoSim(self):
        """Device normal mode without SIM"""
        prop = ['Context.Environment.Connection.CurrentData.isEmergencyMode']
        self.ifceMCE.req_call_state_change("none","normal")
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assert_(returnedProp['Context.Environment.Connection.CurrentData.isEmergencyMode'],False)
                     
    def test_normalModeSim(self):
        """Device normal mode with SIM"""
        prop = ['Context.Environment.Connection.CurrentData.isEmergencyMode']
        self.ifceMCE.req_call_state_change("cellular","normal")
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assert_(returnedProp['Context.Environment.Connection.CurrentData.isEmergencyMode'],False)

class Core(ContextHelpers):
    
    def test_inexistentPropertiesRaiseError(self):
        """Getting invalid properties should raise an error """
        prop = ['Context.Device.Something']
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assertRaises(dbus.DBusException,self.getProp,prop)
        
    def test_inexistentPropertiesUndetermined(self):
        """Getting invalid properties should list the property as undetermined """
        prop = ['Context.Device.Something']
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assert_(undeterminedProp.count(prop),1)

def testRun():
    suiteDevice = unittest.TestLoader().loadTestsFromTestCase(Device)
    suiteEnv = unittest.TestLoader().loadTestsFromTestCase(Environment)
    suiteCore = unittest.TestLoader().loadTestsFromTestCase(Core)
    unittest.TextTestRunner(verbosity=2).run(suiteDevice)
    unittest.TextTestRunner(verbosity=2).run(suiteEnv)
    unittest.TextTestRunner(verbosity=2).run(suiteCore)
