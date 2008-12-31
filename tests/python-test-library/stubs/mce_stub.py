#!/usr/bin/env python2.5
##
## @file mce_stub.py
##
## Copyright (C) 2008 Nokia. All rights reserved.
##
## 
## This python file export a DBUS server, which emulates the 
## objects/interfaces provided by mce daemon.
## It is used for testing contextd
##
## Requires python2.5-gobject and python2.5-dbus
##
## Implements also some testing API:
##
## com.nokia.mce
## com.nokia.mce.request
## 
##
## set_error (string error)
##

import dbus
import dbus.service
import gobject
import getopt
import sys
from dbus.mainloop.glib import DBusGMainLoop


class MCEException(dbus.DBusException):
    def __init__(self, error_name):
        dbus.DBusException.__init__(self)
        self._dbus_error_name = error_name


class MCESignal(dbus.service.Object):
  
    def __init__(self, main_loop):
        # Here the object path
        dbus.service.Object.__init__(self,
                                 dbus.SystemBus(),
                                 "/com/nokia/mce/signal")

    #
    # DBUS signals
    #

    @dbus.service.signal(dbus_interface='com.nokia.mce.signal',
                       signature='s')
    def sig_device_mode_ind(self, mode):
        #print "sig_device_mode_ind(%s)" % (mode)
        pass

    @dbus.service.signal(dbus_interface='com.nokia.mce.signal',
                       signature='s')
    def display_status_ind(self, status):
        #print "display_status_ind(%s)" % (status)
        pass

    @dbus.service.signal(dbus_interface='com.nokia.mce.signal', signature='sssiii')
    def sig_device_orientation_ind(self, rotation, stand, facing, x, y, z):
        #print "sig_device_orientation_ind(%s %s %s %d %d %d)" % (rotation, stand, facing, x, y, z)
        pass

    @dbus.service.signal(dbus_interface='com.nokia.mce.signal', signature='ss')
    def sig_call_state_ind (self, state, type):
        #print "sig_call_state_ind(%s %s)" % (state,type)
        pass
    



# Manager stuff
class MCE(dbus.service.Object):

    error = ""
    device_mode = "normal"
    device_facing = "face_up"
    device_rotation = "portrait"
    device_stand = "on_stand"
    device_x= 0
    device_y= 0
    device_z= 0
    display_state = "on"
    call_state = "cellular"
    call_type = "normal"

    def __init__(self, main_loop):
    # Here the object path
        dbus.service.Object.__init__(self,
                                 dbus.service.BusName("com.nokia.mce", bus=dbus.SystemBus()),
                                 "/com/nokia/mce/request")

        self.main_loop = main_loop
        self.stopped = False
        self.signal_object = MCESignal(self.main_loop)

    #
    # DBUS methods
    #

    @dbus.service.method(dbus_interface='com.nokia.mce.request',
                       in_signature='', out_signature='s')
    def get_device_mode(self):
        if self.error:
            raise MCEException(self.error)

        return self.device_mode

    @dbus.service.method(dbus_interface='com.nokia.mce.request',
                       in_signature='', out_signature='ss')
    def get_call_state(self):
        if self.error:
            raise MCEException(self.error)

        return self.call_state, self.call_type

    @dbus.service.method(dbus_interface='com.nokia.mce.request',
                       in_signature='', out_signature='sssiii')
    def get_device_orientation(self):
        if self.error:
            raise MCEException(self.error)

        return self.device_rotation, self.device_stand, self.device_facing, \
                            self.device_x, self.device_y, self.device_z
    
    @dbus.service.method(dbus_interface='com.nokia.mce.request',
                       in_signature='', out_signature='s')
    def get_display_status(self):
        return self.display_state

    @dbus.service.method(dbus_interface='com.nokia.mce.request',
                       in_signature='s', out_signature='')
    def req_device_mode_change(self, new_mode):
        if self.error:
            raise MCEException(self.error)

        if new_mode not in ("normal", "flight"):
            raise MCEException("org.freedesktop.DBus.Error.InvalidArgs")

        self.device_mode = new_mode
        self.signal_object.sig_device_mode_ind(new_mode)

    @dbus.service.method(dbus_interface='com.nokia.mce.request',
                       in_signature='s', out_signature='')
    def req_device_facing_change(self, new_facing):
        if self.error:
            raise MCEException(self.error)

        if new_facing not in ("face_up", "face_down", "unknown"):
            raise MCEException("org.freedesktop.DBus.Error.InvalidArgs")

        self.device_facing = new_facing
        self.signal_object.sig_device_orientation_ind(self.device_rotation, \
                                  self.device_stand, new_facing, self.device_x, \
                                  self.device_y, self.device_z)

    @dbus.service.method(dbus_interface='com.nokia.mce.request',
                       in_signature='s', out_signature='')
    def req_device_rotation_change(self, new_rotation):
        if self.error:
            raise MCEException(self.error)

        if new_rotation not in ("portrait", "landscape", "unknown"):
            raise MCEException("org.freedesktop.DBus.Error.InvalidArgs")

        self.device_rotation = new_rotation
        self.signal_object.sig_device_orientation_ind(new_rotation, \
                                      self.device_stand, self.device_facing, \
                                      self.device_x, self.device_y, self.device_z)

    @dbus.service.method(dbus_interface='com.nokia.mce.request',
                       in_signature='s', out_signature='')
    def req_device_stand_change(self, new_stand):
        if self.error:
            raise MCEException(self.error)

        if new_stand not in ("on_stand", "off_stand", "unknown"):
            raise MCEException("org.freedesktop.DBus.Error.InvalidArgs")

        self.device_stand = new_stand
        self.signal_object.sig_device_orientation_ind(self.device_rotation, \
                                                      new_stand,self.device_facing)

    @dbus.service.method(dbus_interface='com.nokia.mce.request',
                       in_signature='iii', out_signature='')
    def req_device_coord_change(self, x, y, z):
        if self.error:
            raise MCEException(self.error)

        if ((x not in range(-2000, 2000, 1) or (y not in range (-2000, 2000, 1)) or \
             (z not in range (-2000, 2000, 1)))):
            raise MCEException("org.freedesktop.DBus.Error.InvalidArgs")

        self.device_x = x
        self.device_y = y
        self.device_z = z
        self.signal_object.sig_device_orientation_ind(self.device_rotation,  \
                  self.device_stand, self.device_facing, x, y, z)

    @dbus.service.method(dbus_interface='com.nokia.mce.request',
                       in_signature='', out_signature='')
    def req_display_state_on(self):
        self.display_state = "on" 
        self.signal_object.display_status_ind(self.display_state)

    @dbus.service.method(dbus_interface='com.nokia.mce.request',
                       in_signature='', out_signature='')
    def req_display_state_dim(self):
       self.display_state = "dimmed"
       self.signal_object.display_status_ind(self.display_state) 

    @dbus.service.method(dbus_interface='com.nokia.mce.request',
                       in_signature='', out_signature='')
    def req_display_state_off (self):
        self.display_state = "off"
        self.signal_object.display_status_ind(self.display_state)

    @dbus.service.method(dbus_interface='com.nokia.mce.request',
                       in_signature='ss', out_signature='')
    def req_call_state_change(self, new_state, new_type):
        if new_state not in ("cellular", "voip", "video", "none"):
            raise MCEException("org.freedesktop.DBus.Error.InvalidArgs")
        if new_type not in ("emergency", "normal"):
            raise MCEException("org.freedesktop.DBus.Error.InvalidArgs")
        
        self.call_state = new_state
        self.call_type = new_type
        self.signal_object.sig_call_state_ind(self.call_state,self.call_type)

    @dbus.service.method(dbus_interface='com.nokia.mce.request',
                         in_signature='', out_signature='')
    def Exit(self):
        self.main_loop.quit()
    
def usage(prog="mce_stub"):
    print """
    mce_stub : create DBus service which exposes MCE properties
    
    mce_stub [-E] [-h]
    
    -h                   print this message
    
    -E                   enable state machine to change the values of MCE properties
 """
    
# Main stuff
def main():
    engine = False
    
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hE")
    except getopt.GetoptError, err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        usage()
        sys.exit(2)

    for o, a in opts:
        if o == "-E":
            engine = True
        elif o == "-h":
            usage();
            sys.exit(0)
        else:
            assert False, "unhandled option"
    
    if engine:
        #TODO state machine MCE
        #print "Start state machine"
        pass
    
    DBusGMainLoop(set_as_default=True)
    loop = gobject.MainLoop()
    manager_server = MCE(loop)
    loop.run()

if __name__ == "__main__":
    main()

