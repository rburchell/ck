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

import os
from time import sleep
import dbus
import commands
import dbus_emulator
import conf
import contextGetPropertiesTCs
import contextSubscribeTCs
import test

def module_setUp():
    dbus_emulator.createBus(dbus_emulator.SYSTEM,conf.sessionConfigPath)
    dbus_emulator.createBus(dbus_emulator.SESSION,conf.sessionConfigPath)
    sleep (5)
    os.system(conf.contextSrcPath + os.sep + "tests/python-test-library/stubs/mce_stub.py &")
    sleep (5)
    os.system(conf.contextSrcPath + os.sep + "contextd &")
    #sleep (5)
    #os.system(conf.contextSrcPath + os.sep + "tests/python-test-library/testcases/test.py &")
    #sleep (5)
    

def module_tearDown():
    os.kill(int(commands.getoutput('pidof contextd')),15)
    dbus_emulator.deleteBus(dbus_emulator.SYSTEM)
    dbus_emulator.deleteBus(dbus_emulator.SESSION)

# Main stuff
if __name__ == "__main__":
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

    ifceMCE.req_device_facing_change("face_down")
    ifceMCE.req_device_facing_change("face_up")        

    module_tearDown()