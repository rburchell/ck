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
sys.path.append("/home/lamadon/workspace/ContextKit/tests/python-test-library/testcases")
sys.path.append("/home/lamadon/workspace/ContextKit/tests/python-test-library/stubs")
import os
from time import sleep
import dbus
import commands
import dbus_emulator
import conf
import contextGetPropertiesTCs
import contextSubscriptionTCs
#import contextSubscribePropertiesTCs

from signal import *

def module_setUp():
    print "Creating buses"
    dbus_emulator.createBus(dbus_emulator.SYSTEM,conf.sessionConfigPath)
    dbus_emulator.createBus(dbus_emulator.SESSION,conf.sessionConfigPath)
    #print os.environ['DBUS_SYSTEM_BUS_ADDRESS']
    #print os.environ['DBUS_SESSION_BUS_ADDRESS']
    sleep (5)
    print "Starting MCE stub"
    os.system(conf.contextSrcPath + os.path.sep + "tests/python-test-library/stubs/mce_stub.py &")
    sleep (5)
    print "Starting contextd"
    os.system(conf.contextSrcPath + os.path.sep + "contextd &")
    #sleep (5)
    #print "Starting subscriber handler"
    #os.system(conf.contextSrcPath + os.path.sep + "tests/python-test-library/testcases/contextSubscriberHandler.py &")
    #sleep (5)
    
    #os.system(conf.contextSrcPath + os.sep + "tests/python-test-library/testcases/test.py &")
    #sleep (5)
    

def module_tearDown():
    
    for pid in commands.getoutput("ps -ef | egrep -i contextd | awk '{print $2}'").split() :
        try:
            os.kill(int(pid), SIGQUIT)
        except OSError, e:
            if not e.errno == 3 : raise e
    dbus_emulator.deleteBus(dbus_emulator.SYSTEM)
    dbus_emulator.deleteBus(dbus_emulator.SESSION)
    
# Main stuff
if __name__ == "__main__":
    print "runtests.py main"
    print "Starting tests"
    module_setUp()
    
    contextGetPropertiesTCs.testRun()
    
    #########################################
    
    #sysBus = dbus.SystemBus()
    #try:
    #    object  = sysBus.get_object("com.nokia.mce","/com/nokia/mce/request")
    #    ifceMCE = dbus.Interface(object,"com.nokia.mce.request")
    #except dbus.DBusException:
    #    traceback.print_exc()
    #    sys.exit(1)
    
    #sessionBus = dbus.SessionBus()
    #try:
    #    object  = sessionBus.get_object("com.nokia.test","/com/nokia/test/request")
    #    ifceSub = dbus.Interface(object,"com.nokia.test.request")
    #except dbus.DBusException:
    #    traceback.print_exc()
    #    sys.exit(1)

    #ifceMCE.req_device_facing_change("face_up")
    #ifceMCE.req_device_rotation_change("portrait")
    #ifceMCE.req_device_coord_change(1000, 20, -10)
    
    
    #########################################
    
    #contextSubscribePropertiesTCs.testRun()
    contextSubscriptionTCs.testRun()
    #print "Tearing down tests"
    #while (True):
    #    continue
    module_tearDown()

