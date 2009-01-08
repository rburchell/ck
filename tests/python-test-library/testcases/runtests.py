#!/usr/bin/env python2.5
##
## @file runtests.py
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
sys.path.append("../testcases")
sys.path.append("../stubs")
import os
from time import sleep
import dbus
import commands
import dbus_emulator
import conf
import contextGetPropertiesTCs
#import contextSubscribeTCs
import test


def module_setUp():
    print "Creating buses"
    dbus_emulator.createBus(dbus_emulator.SYSTEM,conf.sessionConfigPath)
    dbus_emulator.createBus(dbus_emulator.SESSION,conf.sessionConfigPath)
    sleep (5)
    print "Starting MCE stub"
    os.system(conf.contextSrcPath + os.path.sep + "tests/python-test-library/stubs/mce_stub.py &")
    sleep (5)
    print "Starting contextd"
    os.system(conf.contextSrcPath + os.path.sep + "contextd &")
    #sleep (5)
    #os.system(conf.contextSrcPath + os.sep + "tests/python-test-library/testcases/test.py &")
    #sleep (5)
    

def module_tearDown():
    os.kill(int(commands.getoutput('pidof contextd')),15)
    dbus_emulator.deleteBus(dbus_emulator.SYSTEM)
    dbus_emulator.deleteBus(dbus_emulator.SESSION)


def get_mce_props():
    # Get the corresponding property from contextd
    edge_key = "Context.Device.Orientation.edgeUp"
    facing_key = "Context.Device.Orientation.facingUp"
    display_key = "Context.Device.Display.displayState"
    flight_key = "Context.Environment.Connection.CurrentData.isFlightMode"
    emergency_key = "Context.Environment.Connection.CurrentData.isEmergencyMode"
    activity_key = "Context.Device.Usage.inActiveUse";
    
    props = []
    
    sesBus = dbus.SessionBus()
    try:
        object  = sesBus.get_object("org.freedesktop.ContextKit","/org/freedesktop/ContextKit/Manager")
        iface = dbus.Interface(object,"org.freedesktop.ContextKit.Manager")
        ret = iface.Get([edge_key, facing_key, display_key, flight_key, emergency_key, activity_key])        
            
        props = [ret[edge_key][1], ret[facing_key][1], ret[display_key][1], ret[flight_key][1], ret[emergency_key][1], ret[activity_key][1]]
        
    except dbus.DBusException:
        print "Caught an exception"
        traceback.print_exc()
    
    return props

def check_props(props, correct):
    print props
    
    if len(props) != len(correct):
        print "Wrong length of properties", props
        return False
    
    for i in range(len(props)):
        if props[i] != correct[i]:
            print "Property failed, they are:", props
            return False
    return True

def execute_get_tests(ifceMCE):
    # Facing down
    ifceMCE.req_device_facing_change("face_down")

    props = get_mce_props()
    if check_props(props, [0, 2, 1, False, False, True]) == True:
        print "## SUCCESS ##"
    else:
    	print "## FAIL ##"
    
    # Facing up
    ifceMCE.req_device_facing_change("face_up")   
    
    props = get_mce_props()
    if check_props(props, [0, 1, 1, False, False, True]) == True:
        print "## SUCCESS ##"
    else:
    	print "## FAIL ##"
    	
    # FIXME: Add orientation tests
   
    # Dim display
    ifceMCE.req_display_state_dim()
    
    props = get_mce_props()
    if check_props(props, [0, 1, 2, False, False, True]) == True:
        print "## SUCCESS ##"
    else:
    	print "## FAIL ##"
    
    # Display off
    ifceMCE.req_display_state_off()
    
    props = get_mce_props()
    if check_props(props, [0, 1, 0, False, False, True]) == True:
        print "## SUCCESS ##"
    else:
    	print "## FAIL ##"
    
    # Display on again
    ifceMCE.req_display_state_on()
    
    props = get_mce_props()
    if check_props(props, [0, 1, 1, False, False, True]) == True:
        print "## SUCCESS ##"
    else:
    	print "## FAIL ##"

    # Flight mode
    ifceMCE.req_device_mode_change("flight")
    props = get_mce_props()
    if check_props(props, [0, 1, 1, True, False, True]) == True:
        print "## SUCCESS ##"
    else:
    	print "## FAIL ##"
    	
    # Normal mode again
    ifceMCE.req_device_mode_change("normal")
    props = get_mce_props()
    if check_props(props, [0, 1, 1, False, False, True]) == True:
        print "## SUCCESS ##"
    else:
    	print "## FAIL ##"
    
    # Normal call
    ifceMCE.req_call_state_change("cellular", "normal")
    props = get_mce_props()
    if check_props(props, [0, 1, 1, False, False, True]) == True:
        print "## SUCCESS ##"
    else:
    	print "## FAIL ##"
    
    # Normal call ends
    ifceMCE.req_call_state_change("none", "normal")
    props = get_mce_props()
    if check_props(props, [0, 1, 1, False, False, True]) == True:
        print "## SUCCESS ##"
    else:
    	print "## FAIL ##"
    	
    # Emergency call
    ifceMCE.req_call_state_change("cellular", "emergency")
    props = get_mce_props()
    if check_props(props, [0, 1, 1, False, True, True]) == True:
        print "## SUCCESS ##"
    else:
    	print "## FAIL ##"
    
    # Emergency call ends
    ifceMCE.req_call_state_change("none", "emergency")
    props = get_mce_props()
    if check_props(props, [0, 1, 1, False, False, True]) == True:
        print "## SUCCESS ##"
    else:
    	print "## FAIL ##"
    	
    # Device inactive
    ifceMCE.req_inactivity_status_inactive()
    props = get_mce_props()
    if check_props(props, [0, 1, 1, False, False, False]) == True:
        print "## SUCCESS ##"
    else:
    	print "## FAIL ##"
    
    # Device active again
    ifceMCE.req_inactivity_status_active()
    props = get_mce_props()
    if check_props(props, [0, 1, 1, False, False, True]) == True:
        print "## SUCCESS ##"
    else:
    	print "## FAIL ##"

def execute_subscribe_tests(ifceMCE):
    # FIXME: We need the unsubscription to work to test this, right?
    

# Main stuff
if __name__ == "__main__":
    print "runtests.py main"
    print "Starting tests"
    module_setUp()
    #contextGetPropertiesTCs.testRun()
    #test.testRun()
    #module_tearDown()
    
    sysBus = dbus.SystemBus()
    try:
        object  = sysBus.get_object("com.nokia.mce","/com/nokia/mce/request")
        ifceMCE = dbus.Interface(object,"com.nokia.mce.request")
    except dbus.DBusException:
        traceback.print_exc()
        sys.exit(1)

    execute_get_tests(ifceMCE)
    
    execute_subscribe_tests(ifceMCE)
    
    print "Tearing down tests"
    module_tearDown()
