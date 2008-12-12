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
from dbus.mainloop.glib import DBusGMainLoop
import gobject

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
    print "sig_device_mode_ind(%s)" % (mode)

  @dbus.service.signal(dbus_interface='com.nokia.mce.signal', signature='sss')
  def sig_device_orientation_ind(self, rotation, stand, facing):
    print "sig_device_orientation_ind(%s %s %s)" % (rotation, stand, facing)

# Manager stuff
class MCE(dbus.service.Object):

  error = ""
  device_mode = "normal"
  device_facing = "face_up"
  device_rotation = "portrait"
  device_stand = "on_stand"
  device_x= 10
  device_y= 70
  device_z= 30

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
                       in_signature='', out_signature='sssiii')
  def get_device_orientation(self):
    if self.error:
      raise MCEException(self.error)

    return self.device_rotation,self.device_stand,self.device_facing,self.device_x, self.device_y, self.device_z

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
    self.signal_object.sig_device_orientation_ind(self.device_rotation,self.device_stand,new_facing)

  @dbus.service.method(dbus_interface='com.nokia.mce.request',
                       in_signature='s', out_signature='')
  def req_device_rotation_change(self, new_rotation):
    if self.error:
      raise MCEException(self.error)

    if new_rotation not in ("portrait", "landscape", "unknown"):
      raise MCEException("org.freedesktop.DBus.Error.InvalidArgs")

    self.device_rotation = new_rotation
    self.signal_object.sig_device_orientation_ind(new_rotation,self.device_stand,self.device_facing)

  @dbus.service.method(dbus_interface='com.nokia.mce.request',
                       in_signature='s', out_signature='')
  def req_device_stand_change(self, new_stand):
    if self.error:
      raise MCEException(self.error)

    if new_stand not in ("on_stand", "off_stand", "unknown"):
      raise MCEException("org.freedesktop.DBus.Error.InvalidArgs")

    self.device_stand = new_stand
    self.signal_object.sig_device_orientation_ind(self.device_rotation,new_stand,self.device_facing)



  @dbus.service.method(dbus_interface='com.nokia.mce.request',
                       in_signature='', out_signature='s')
  def get_display_status(self):
    return "on"

  @dbus.service.method(dbus_interface='com.nokia.mce.request',
                         in_signature='', out_signature='')
  def Exit(self):
        loop.quit()
    

# Main stuff
if __name__ == "__main__":
  DBusGMainLoop(set_as_default=True)

  loop = gobject.MainLoop()
  manager_server = MCE(loop)
  loop.run()

