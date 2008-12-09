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

import sys
import os
import re
import getopt

SYSTEM = 6
SESSION = 5

def parse (pattern, string):
    result = re.match(pattern, string)
    if result != None:
        return result
    else:
        raise Exception(pattern+" not found in "+string+" quit dbus emulator")
        sys.exit(1)

def createBus(type):
    output = None
    if ((os.getenv("DBUS_SYSTEM_BUS_ADDRESS") == None and type == SYSTEM) or 
            (os.getenv("DBUS_SESSION_BUS_ADDRESS") == None and type == SESSION)):
        output = os.popen("dbus-launch").readlines()
        if len(output) == 2:
            result = parse("(DBUS_SESSION_BUS_ADDRESS)=(.*)",output[0]) , parse("(DBUS_SESSION_BUS_PID)=(.*)",output[1])
            if type == SESSION:
                os.environ['DBUS_SESSION_BUS_ADDRESS'] = result[0].group(2)
                os.environ['DBUS_SESSION_BUS_PID'] = result[1].group(2)
            
            if type == SYSTEM: 
                os.environ['DBUS_SYSTEM_BUS_ADDRESS'] = result[0].group(2)
                os.environ['DBUS_SYSTEM_BUS_PID'] = result[1].group(2)
    else:
        if type == SYSTEM:
            print "Instance of system bus already exists"
        else:
            print "Instance of session bus already exists"
        sys.exit(0)
    
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
            createBus(SYSTEM)
        elif o == "-s":
            createBus(SESSION)
        elif o == "-h":
            usage();
            sys.exit(0)
        else:
            assert False, "unhandled option"

if __name__ == "__main__":
    main()
            