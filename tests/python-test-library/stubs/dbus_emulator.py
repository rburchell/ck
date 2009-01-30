#!/usr/bin/env python2.5
##
## @system_bus_emulator.py
##
## Copyright (C) 2008 Nokia. All rights reserved.
##
## Create session bus and emulate real session or system bus 
##
## Requires python2.5-gobject and python2.5-dbus
##
##
import signal
import sys
import os
import re
import getopt
from time import sleep
import ConfigParser

SYSTEM = 6
SESSION = 5

def readConfig(configFilePath):
    config = ConfigParser.RawConfigParser()
    config.read(configFilePath)
    sections = config.sections()
    return config.items(sections[0])
        
def writeConfig(section, keyValuePairs, configFilePath):
    config = ConfigParser.RawConfigParser()
    
    config.add_section(section)
    for keyValuePair in keyValuePairs:
        config.set(section, keyValuePair[0], keyValuePair[1])
    
    f = open(configFilePath,"ab")
    config.write(f)
    f.close

def parse (pattern, string):
    result = re.match(pattern, string)
    if result != None:
        return result
    else:
        raise Exception(pattern+" not found in "+string+" quit dbus emulator")
        sys.exit(1)

def createBus(type,configPath):
    
    output = None
    if ((os.getenv("DBUS_SYSTEM_BUS_ADDRESS") == None and type == SYSTEM) or type == SESSION):
        #if os.path.isdir(configPath):
        #    output = os.popen("dbus-launch --config-file=" + configPath + os.sep + "session.conf").readlines()
        #else:
        output = os.popen("dbus-launch").readlines()
        if len(output) >= 2:
            result = parse("(DBUS_SESSION_BUS_ADDRESS)=(.*)",output[0]) , parse("(DBUS_SESSION_BUS_PID)=(.*)",output[1])
            if type == SESSION:
                os.environ['DBUS_SESSION_BUS_ADDRESS'] = result[0].group(2)
                os.environ['DBUS_SESSION_BUS_PID'] = result[1].group(2)
                keyPairs = [('DBUS_SESSION_BUS_ADDRESS',result[0].group(2)),('DBUS_SESSION_BUS_PID',result[1].group(2))]
                writeConfig("Session", keyPairs, "/tmp/dbus")
                                
            if type == SYSTEM: 
                os.environ['DBUS_SYSTEM_BUS_ADDRESS'] = result[0].group(2)
                os.environ['DBUS_SYSTEM_BUS_PID'] = result[1].group(2)
                keyPairs = [('DBUS_SYSTEM_BUS_ADDRESS',result[0].group(2)),('DBUS_SYSTEM_BUS_PID',result[1].group(2))]
                writeConfig("Session", keyPairs, "/tmp/dbus")
                
        #print os.environ['DBUS_SYSTEM_BUS_ADDRESS']
        #print os.environ['DBUS_SESSION_BUS_ADDRESS']
    else:
        if type == SYSTEM:
            print "Instance of system bus already exists"
        else:
            print "Instance of session bus already exists"

def deleteBus(type):
    if type == SYSTEM:
        os.kill(int(os.getenv("DBUS_SYSTEM_BUS_PID")),15)
    elif type == SESSION:
        os.kill(int(os.getenv("DBUS_SESSION_BUS_PID")),15)
    #os.remove("/tmp/dbus")
 
def usage(prog="dbus_emulator"):
    print """
    dbus_emulator : create session bus which will emulate real session or system
    bus
    
    dbus_emulator [-S] [-s] [-h]
    
    -h                   print this message
    
    -S                   create system bus instance 
    
    -s                   create session bus instance

 """
    
# Main stuff
def main():

    try:
        opts, args = getopt.getopt(sys.argv[1:], "hSs")
    except getopt.GetoptError, err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        usage()
        sys.exit(2)

    for o, a in opts:
        if o == "-S":
            createBus(SYSTEM,"")
        elif o == "-s":
            createBus(SESSION,"")
        elif o == "-h":
            usage();
            sys.exit(0)
        else:
            assert False, "unhandled option"

if __name__ == "__main__":
    main()
            
