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
sys.path.append("tests/python-test-library/testcases")
sys.path.append("tests/python-test-library/stubs")
import os
from time import sleep
import dbus
import commands
import dbus_emulator
import conf
import contextGetPropertiesTCs
import contextSubscriberHandler
from signal import *

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
    sleep (5)
    #print "Starting subscriber handler"
    #os.system(conf.contextSrcPath + os.path.sep + "tests/python-test-library/testcases/contextSubscriberHandler.py &")
    #sleep (5)
    
    #os.system(conf.contextSrcPath + os.sep + "tests/python-test-library/testcases/test.py &")
    #sleep (5)
    

def module_tearDown():
    
    dbus_emulator.deleteBus(dbus_emulator.SYSTEM)
    dbus_emulator.deleteBus(dbus_emulator.SESSION)
    for pid in commands.getoutput("ps -ef | egrep -i contextd | awk '{print $2}'").split() :
        try:
            os.kill(int(pid), SIGQUIT)
        except OSError, e:
            if not e.errno == 3 : raise e

# Main stuff
if __name__ == "__main__":
    print "runtests.py main"
    print "Starting tests"
    module_setUp()
    contextGetPropertiesTCs.testRun()
    #contextSubscribePropertiesTCs.testRun()
    print "Tearing down tests"
    module_tearDown()

