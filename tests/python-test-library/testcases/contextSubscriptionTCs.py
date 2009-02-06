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
import sys
sys.path.append("./tests/python-test-library/stubs")
import conf as cfg
import os
from time import sleep
import unittest
import traceback
import dbus
import dbus_emulator
import ConfigParser
import commands
from signal import *
import contextSubscriberHandler as ctxHandler

class Subscriber():
    
    def __init__(self):
        self.bus = dbus.SessionBus()
        try:
            self.mgr = self.bus.get_object(cfg.ctxBusName, cfg.ctxMgrPath)
            self.ifceMgr = dbus.Interface(self.mgr, cfg.ctxMgrIfce)
        except dbus.DBusException:
            pass

        self.scrberPath = self.ifceMgr.GetSubscriber()
                
    def getObjPath(self):
        return self.scrberPath
    
    def getSubProxy(self):
        return self.bus.get_object(cfg.ctxBusName,self.scrberPath)
    
    def closeBus(self):
        try:
            self.bus.close()
        except dbus.DBusException:
            pass
    
    def __del__(self):
        try:
            self.bus.close()
        except dbus.DBusException:
            pass
            #traceback.print_exc()
            
class TestSubscriber (unittest.TestCase):
    
    def setUp(self):
        #dbus_emulator.createBus(dbus_emulator.SESSION,cfg.sessionConfigPath)
        #sleep (4)
        #os.system(cfg.contextSrcPath + os.path.sep + "contextd &")
        #sleep (4)
        #print os.environ['DBUS_SESSION_BUS_ADDRESS']

        #config = ConfigParser.RawConfigParser()
        #config.read("../stubs/dbus")
        #sections = config.sections()
        #for section in sections:
        #    for item in config.items(section):
        #        if item[0] == 'dbus_system_bus_address':
        #            os.environ['DBUS_SYSTEM_BUS_ADDRESS'] = item[1] 
        #        elif item[0] == 'dbus_session_bus_address':
        #            os.environ['DBUS_SESSION_BUS_ADDRESS'] = item[1]
        pass
    
    def tearDown(self):
        #for pid in commands.getoutput("ps -Af | egrep -i contextd | awk '{print $2}'").split() :
        #    try:
        #        os.kill(int(pid), SIGQUIT)
        #    except OSError, e:
        #        if not e.errno == 3 : raise e
        #dbus_emulator.deleteBus(dbus_emulator.SESSION)
        pass
        #config = ConfigParser.RawConfigParser()
        #config.read("/tmp/dbus")
        #sections = config.sections()
        #for section in sections:
        #    for item in config.items(section):
        #        if item[0] == 'dbus_system_bus_pid' or item[0] == 'dbus_session_bus_pid':
        #            os.kill(int(item[1]),15)
    
    def testAddSubscriber(self):
        self.subscriber = Subscriber()
        self.assert_ ( str(self.subscriber.getObjPath()) == cfg.scriberOnePath, 
                     "First subscriber id is incorrect")
    
#    def testCounterOverflow(self):
#        for i in range(2147483648):
#            self.subscriber = Subscriber()
#            self.subscriber = None
#        self.assertRaises(dbus.DBusException,Subscriber)
    
    def testCounter(self):
        self.subscriber1 = Subscriber()
        self.assert_ ( str(self.subscriber1.getObjPath()) == cfg.scriberOnePath, 
                     "First subscriber id is incorrect")
        self.subscriber1 = None
        self.subscriber2 = Subscriber()
        self.subscriber2 = None
        self.subscriber3 = Subscriber()
        self.assert_ ( str(self.subscriber3.getObjPath()) == cfg.scriberThirdPath, 
                     "Third subscriber id is incorrect")
    
    def testDeleteSubscriber(self):
        exception = False
        self.subscriber = Subscriber()
        path = self.subscriber.getObjPath()
        remoteSub = self.subscriber.getSubProxy()
        self.subscriber.closeBus()
        bus = dbus.SessionBus()
        self.assertRaises(dbus.DBusException,remoteSub.Introspect)

class TestCore(unittest.TestCase):
    
    def setUp(self):
        sysBus = dbus.SystemBus()
        try:
            mce_proxy_obj  = sysBus.get_object(cfg.mceBusName,cfg.mceRqstPath)
            self.ifceMCE = dbus.Interface(mce_proxy_obj, cfg.mceRqstIfce)
        except dbus.DBusException:
            pass
        
        sessionBus = dbus.SessionBus()
        try:
            test_proxy_obj = sessionBus.get_object(cfg.scriberBusName,cfg.scriberHandlerPath)
            self.ifceTest = dbus.Interface(test_proxy_obj, cfg.scriberHandlerIfce)
        except dbus.DBusException:
            print "dbus exception"
            pass
    
    def tearDown(self):
        pass
    
    def testSubscribeProperty(self):
        self.subscriber = self.ifceTest.addSubscriber(True, cfg.ctxBusName)
        self.ifceMCE.req_device_facing_change("face_up")
        self.ifceMCE.req_device_rotation_change("portrait")
        self.ifceMCE.req_device_coord_change(1000, 20, -10)
        properties, undetermined = self.ifceTest.subscribe(cfg.properties,self.subscriber)
        self.ifceMCE.req_device_facing_change("face_down")
        self.ifceMCE.req_device_rotation_change("landscape")
        self.ifceMCE.req_device_coord_change(20, 1000, 10)
        prop, undet = self.ifceTest.getChangedProp(self.subscriber)
        self.assertEqual(properties['Context.Device.Orientation.facingUp'],1)
        self.assertEqual(properties['Context.Device.Orientation.edgeUp'],3)
        self.assertEqual(len(undetermined),0)
        self.assertEqual(prop['Context.Device.Orientation.facingUp'],2)
        self.assertEqual(prop['Context.Device.Orientation.edgeUp'],4)
        self.assertEqual(len(undet),0)
        
    def testUnsubscribeProperty(self):
        subscriber = self.ifceTest.getSubscriber()
        properties, undetermined = self.ifceTest.subscribe(cfg.properties)
        self.ifceTest.unsubscribe()
        
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
        self.assertEqual(returnedProp['Context.Device.Orientation.facingUp'],1)

    def test_facedown(self):
        """Device orientation to face down"""
        self.ifceMCE.req_device_facing_change("face_down")
        prop = ['Context.Device.Orientation.facingUp']
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assertEqual(returnedProp['Context.Device.Orientation.facingUp'],2)
        
    def test_orientationEdgeLeft(self):
        """Device orientation to edge left"""
        self.ifceMCE.req_device_facing_change("face_up")
        self.ifceMCE.req_device_rotation_change("portrait")
        self.ifceMCE.req_device_coord_change(-1000, 20, -10)
        prop = ['Context.Device.Orientation.facingUp','Context.Device.Orientation.edgeUp']
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assertEqual(returnedProp['Context.Device.Orientation.facingUp'],1)
        self.assertEqual(returnedProp['Context.Device.Orientation.edgeUp'],"2")
    
    def test_orientationEdgeRight(self):
        """Device orientation to edge right"""
        self.ifceMCE.req_device_facing_change("face_up")
        self.ifceMCE.req_device_rotation_change("portrait")
        self.ifceMCE.req_device_coord_change(1000, 20, -10)
        prop = ['Context.Device.Orientation.facingUp','Context.Device.Orientation.edgeUp']
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assertEqual(returnedProp['Context.Device.Orientation.facingUp'],1)
        self.assertEqual(returnedProp['Context.Device.Orientation.edgeUp'],"3")
        
    def test_orientationEdgeUp(self):
        """Device orientation to edge up"""
        self.ifceMCE.req_device_facing_change("face_down")
        self.ifceMCE.req_device_rotation_change("landscape")
        self.ifceMCE.req_device_coord_change(20, -1000, 10)
        prop = ['Context.Device.Orientation.facingUp','Context.Device.Orientation.edgeUp']
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assertEqual(returnedProp['Context.Device.Orientation.facingUp'],2)
        self.assertEqual(returnedProp['Context.Device.Orientation.edgeUp'],"1")
    
    def test_orientationEdgeDown(self):
        """Device orientation to edge down"""
        self.ifceMCE.req_device_facing_change("face_down")
        self.ifceMCE.req_device_rotation_change("landscape")
        self.ifceMCE.req_device_coord_change(20, 1000, 10)
        prop = ['Context.Device.Orientation.facingUp','Context.Device.Orientation.edgeUp']
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assertEqual(returnedProp['Context.Device.Orientation.facingUp'],2)
        self.assertEqual(returnedProp['Context.Device.Orientation.edgeUp'],"4")
    
    def test_orientationEdgeFaceUndefined(self):
        """Device orientation to undefined edge and facing"""
        self.ifceMCE.req_device_facing_change("unknown")
        self.ifceMCE.req_device_rotation_change("unknown")
        self.ifceMCE.req_device_coord_change(0, 0, 0)
        prop = ['Context.Device.Orientation.facingUp','Context.Device.Orientation.edgeUp']
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assertEqual(returnedProp['Context.Device.Orientation.facingUp'],0)
        self.assertEqual(returnedProp['Context.Device.Orientation.edgeUp'],"0")
        
    def test_displayStateOn(self):
        """Device display active"""
        prop = ['Context.Device.Display.displayState']
        self.ifceMCE.req_display_state_on()
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assertEqual(returnedProp['Context.Device.Display.displayState'],1)
        
    def test_displayStateOff(self):
        """Device display inactive"""
        prop = ['Context.Device.Display.displayState']
        self.ifceMCE.req_display_state_off()
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assertEqual(returnedProp['Context.Device.Display.displayState'],0)
        
    def test_displayStateDim(self):
        """Device display dim"""
        prop = ['Context.Device.Display.displayState']
        self.ifceMCE.req_display_state_dim()
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assertEqual(returnedProp['Context.Device.Display.displayState'],2)
        
    def test_inActiveUseOff(self):
        """Device disactive"""
        prop = ['Context.Device.Usage.inActiveUse']
        self.ifceMCE.req_inactivity_status_active()
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assertEqual(returnedProp['Context.Device.Usage.inActiveUse'],True)
        
    def test_inActiveUseOn(self):
        """Device active"""
        prop = ['Context.Device.Usage.inActiveUse']
        self.ifceMCE.req_inactivity_status_inactive()
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assertEqual(returnedProp['Context.Device.Usage.inActiveUse'],False)
                

class Environment(ContextHelpers):
    def test_flightModeOn(self):
        """Device mode to flight"""
        prop = ['Context.Environment.Connection.CurrentData.isFlightMode']
        self.ifceMCE.req_device_mode_change("flight")
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assertEqual(returnedProp['Context.Environment.Connection.CurrentData.isFlightMode'],True)
    
    def test_flightModeOff(self):
        """Device mode disable flight mode"""
        prop = ['Context.Environment.Connection.CurrentData.isFlightMode']
        self.ifceMCE.req_device_mode_change("normal")
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assertEqual(returnedProp['Context.Environment.Connection.CurrentData.isFlightMode'],False)
    
    def test_emergencyModeNoSim(self):
        """Device emergency mode without SIM"""
        prop = ['Context.Environment.Connection.CurrentData.isEmergencyMode']
        self.ifceMCE.req_call_state_change("none","emergency")
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assertEqual(returnedProp['Context.Environment.Connection.CurrentData.isEmergencyMode'],False)
                     
    def test_emergencyModeSim(self):
        """Device emergency mode with SIM"""
        prop = ['Context.Environment.Connection.CurrentData.isEmergencyMode']
        self.ifceMCE.req_call_state_change("cellular","emergency")
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assertEqual(returnedProp['Context.Environment.Connection.CurrentData.isEmergencyMode'],True)

    def test_normalModeNoSim(self):
        """Device normal mode without SIM"""
        prop = ['Context.Environment.Connection.CurrentData.isEmergencyMode']
        self.ifceMCE.req_call_state_change("none","normal")
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assertEqual(returnedProp['Context.Environment.Connection.CurrentData.isEmergencyMode'],False)
                     
    def test_normalModeSim(self):
        """Device normal mode with SIM"""
        prop = ['Context.Environment.Connection.CurrentData.isEmergencyMode']
        self.ifceMCE.req_call_state_change("cellular","normal")
        returnedProp, undeterminedProp = self.getProp(prop)
        self.assertEqual(returnedProp['Context.Environment.Connection.CurrentData.isEmergencyMode'],False)

        
def testRun():
    #suiteSubscriber = unittest.TestLoader().loadTestsFromTestCase(TestSubscriber)
    suiteCore = unittest.TestLoader().loadTestsFromTestCase(TestCore)
#    suiteDevice = unittest.TestLoader().loadTestsFromTestCase(Device)
#    suiteEnv = unittest.TestLoader().loadTestsFromTestCase(Environment)
    #unittest.TextTestRunner(verbosity=2).run(suiteSubscriber)
    unittest.TextTestRunner(verbosity=2).run(suiteCore)
#    unittest.TextTestRunner(verbosity=2).run(suiteDevice)
#    unittest.TextTestRunner(verbosity=2).run(suiteEnv)
    
