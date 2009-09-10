#!/usr/bin/env python2.5
##
## @file bluez_stub.py
##
## Copyright (C) 2008 Nokia. All rights reserved.
##
## @author Aapo Makela <aapo.makela@nokia.com>
##
## This python file export a DBUS server, which emulates the 
## objects/interfaces provided by bluez-utils daemon(s).
## It is used for testing conbtui API.
##
## Requires python2.5-gobject and python2.5-dbus
##
## Implements also some testing API:
##
## org.bluez
## /
## org.bluez.Manager.Testing
##
## AddAdapter (string bt_address): objpath
##
## RemoveAdapter (object adapter)
##
## SetDefaultAdapter (object adapter)
##
## SetError (string error)
##
## org.bluez
## /hci{0, 1, ...}
## org.bluez.Adapter.Testing
##
## AddDeviceAround (string bt_address, string name, Uint32 cod, array{string} UUIDs)
##
## TestAgentRequest (string what ("PinCode", "Passkey", "Confirmation"))
##
## SetError (string error)
##
## org.bluez
## /hci{0, 1, ...}/dev_XX_XX_XX_XX_XX_XX
## org.bluez.Device.Testing
##
## Connect ()
##
## SetError (string error)
##

import time
import dbus
import dbus.service
from dbus.mainloop.glib import DBusGMainLoop
import gobject

class BluezException(dbus.DBusException):
  def __init__(self, error_name):
    dbus.DBusException.__init__(self)
    self._dbus_error_name = "org.bluez.Error." + error_name

class BluetoothDevice:

  def __init__(self, bt_address, name = "", cod = 0,
               uuids = dbus.Array(signature="s")):
    # Set properties
    self.properties = {}
    self.properties["Address"] = bt_address
    self.properties["Name"] = name
    self.properties["Class"] = dbus.UInt32(cod)
    self.properties["Paired"] = dbus.Boolean(False)
    self.properties["Connected"] = dbus.Boolean(False)
    self.properties["Trusted"] = dbus.Boolean(False)
    self.properties["Alias"] = self.properties["Name"]
    self.properties["UUIDs"] = uuids

  def __cmp__(self, other):
    return cmp(self.properties["Address"], other.properties["Address"])

# Device stuff
class BluezDevice(BluetoothDevice, dbus.service.Object):
  name = ""
  path = ""
  error = ""

  def __init__(self, main_loop, adapter, bt_address, properties = {}):
    # Here the object path
    self.path = adapter + "/dev_" + bt_address.replace(":", "_")

    BluetoothDevice.__init__(self, bt_address)
    dbus.service.Object.__init__(self, dbus.SystemBus(), self.path)

    self.properties = {}
    if properties:
      self.properties = properties
    self.properties["Adapter"] = dbus.ObjectPath(adapter)
    self.main_loop = main_loop
    self.stopped = False

  @dbus.service.method(dbus_interface='org.bluez.Device',
                       in_signature='', out_signature='a{sv}')
  def GetProperties(self):
    if self.error:
      raise BluezException(self.error)

    return self.properties

  @dbus.service.method(dbus_interface='org.bluez.Device',
                       in_signature='sv', out_signature='')
  def SetProperty(self, name, value):
    if self.error:
      raise BluezException(self.error)

    if name not in self.properties.keys():
      raise BluezException("DoesNotExist")
    if name in ("Address", "Name", "UUIDs", "Paired", "Connected", "Adapter"):
      raise BluezException("InvalidArguments")

    if name == "Alias" and value == "":
      self.properties[name] = self.properties["Name"]
      self.PropertyChanged(name, self.properties["Name"])
    else:
      self.properties[name] = value
      self.PropertyChanged(name, value)

  @dbus.service.method(dbus_interface='org.bluez.Device',
                       in_signature='', out_signature='')
  def Disconnect(self):
    if self.error:
      raise BluezException(self.error)

    if self.properties["Connected"] == False:
      raise BluezException("NotConnected")

    self.DisconnectRequested()
    time.sleep(2)
    self.properties["Connected"] = False
    self.PropertyChanged("Connected", False)

  #
  # DBUS signals
  #

  @dbus.service.signal(dbus_interface='org.bluez.Device',
                       signature='')
  def DisconnectRequested(self):
    print "device DisconnectedRequested signal"

  @dbus.service.signal(dbus_interface='org.bluez.Device',
                       signature='sv')
  def PropertyChanged(self, name, value):
    print "device PropertyChanged(%s, %s) signal" % (str(name), str(value))

  #
  # DBUS test methods
  #

  @dbus.service.method(dbus_interface='org.bluez.Device.Testing',
                       in_signature='', out_signature='')
  def Connect(self):
    self.properties["Connected"] = True
    self.PropertyChanged("Connected", True)

  @dbus.service.method(dbus_interface='org.bluez.Device.Testing',
                       in_signature='s', out_signature='')
  def SetError(self, error):
    self.error = error
  

# Adapter stuff
class BluezAdapter(dbus.service.Object):
  
  properties = {}
  name = ""
  global_agent = None
  devices_around = [BluetoothDevice("11:22:33:44:55:66", "Phone", 5898764),
                    BluetoothDevice("22:33:44:55:66:77", "Tablet", 1048852)]
  devices = {}
  stopped = True
  error = ""

  def __init__(self, main_loop, path, bt_address):
    # Here the object path
    self.path = path
    dbus.service.Object.__init__(self, dbus.SystemBus(), self.path)

    self.main_loop = main_loop
    self.stopped = False

    # Set properties
    self.properties["Address"] = bt_address
    self.properties["Name"] = "Test"
    self.properties["Powered"] = False
    self.properties["Discoverable"] = False
    self.properties["DiscoverableTimeout"] = dbus.UInt32(180)
    self.properties["Discovering"] = dbus.Boolean(False)

    self.pairing_return_cb = None
    self.pairing_error_cb = None
    self.new_device = None
    self.agent = None
    self.timeout = 0
    self.sessions = 0

  @dbus.service.method(dbus_interface='org.bluez.Adapter',
                       in_signature='', out_signature='a{sv}')
  def GetProperties(self):
    if self.error:
      raise BluezException(self.error)

    return self.properties

  @dbus.service.method(dbus_interface='org.bluez.Adapter',
                       in_signature='sv', out_signature='')
  def SetProperty(self, name, value):
    if self.error:
      raise BluezException(self.error)

    if name not in self.properties.keys():
      raise BluezException("DoesNotExist")
    if name in ("Address"):
      raise BluezException("InvalidArguments")
    self.properties[name] = value
    self.PropertyChanged(name, value)

  @dbus.service.method(dbus_interface='org.bluez.Adapter',
                       in_signature='', out_signature='')
  def RequestSession(self):
    if self.sessions == 0 and not self.properties["Powered"]:
        if self.global_agent:
            try:
                if self.properties["Discoverable"]:
                    self.global_agent.ConfirmModeChange("discoverable")
                else:
                    self.global_agent.ConfirmModeChange("connectable")
            except dbus.DBusException:
                raise BluezException("Rejected")
        self.properties["Powered"] = True
        self.PropertyChanged("Powered", True)
    self.sessions += 1

  @dbus.service.method(dbus_interface='org.bluez.Adapter',
                       in_signature='', out_signature='')
  def ReleaseSession(self):
    self.sessions -= 1
    if self.sessions == 0:
        self.properties["Powered"] = False
        self.PropertyChanged("Powered", False)
    if self.sessions < 0:
        self.sessions = 0

  def discover_device_timeout(self):
    if self.stopped == True or self.idx >= len(self.devices_around):
      self.timeout = gobject.timeout_add(30000, self.discover_device_timeout)
      return False

    properties = self.devices_around[self.idx].properties.copy()
    properties["RSSI"] = dbus.Int16(-60)
    if properties.has_key("UUIDs"):
      del properties["UUIDs"]
    self.DeviceFound(properties["Address"], properties)
    self.idx += 1
    return True

  @dbus.service.method(dbus_interface='org.bluez.Adapter',
                       in_signature='', out_signature='')
  def StartDiscovery(self):
    if self.error:
      raise BluezException(self.error)

    self.idx = 0
    self.stopped = False
    self.timeout = gobject.timeout_add(3000, self.discover_device_timeout)
    self.SetProperty("Discovering", dbus.Boolean(True))

  @dbus.service.method(dbus_interface='org.bluez.Adapter',
                       in_signature='', out_signature='')
  def StopDiscovery(self):
    if self.error:
      raise BluezException(self.error)

    self.stopped = True
    if self.timeout:
      self.SetProperty("Discovering", dbus.Boolean(False))
      gobject.source_remove(self.timeout)

  @dbus.service.method(dbus_interface='org.bluez.Adapter',
                       in_signature='s', out_signature='o')
  def FindDevice(self, address):
    if self.error:
      raise BluezException(self.error)

    for path, device in self.devices.items():
      if device.properties["Address"] == address:
        return path
    raise BluezException("DoesNotExist")

  @dbus.service.method(dbus_interface='org.bluez.Adapter',
                       in_signature='', out_signature='ao')
  def ListDevices(self):
    if self.error:
      raise BluezException(self.error)

    keys = self.devices.keys()
    keys.sort()
    return keys

  @dbus.service.method(dbus_interface='org.bluez.Adapter',
                       in_signature='s', out_signature='')
  def CancelDeviceCreation(self, address):
    pass

  @dbus.service.method(dbus_interface='org.bluez.Adapter',
                       in_signature='s', out_signature='o')
  def CreateDevice(self, address):
    if self.error:
      raise BluezException(self.error)

    for device in self.devices_around:
      if device.properties["Address"] == address:
        new_device = BluezDevice(self.main_loop, self.path, 
                                 device.properties["Address"],
                                 device.properties)
        self.devices[new_device.path] = new_device
        self.DeviceCreated(new_device.path)
        # TODO: add UUIDs
        return new_device.path
    raise BluezException("Failed")

  def pincode_reply(self, passkey):
    print "pincode_reply %s" % str(passkey)
    self.passkey = passkey
    self.stopped = True

    if self.passkey != "1234":
      self.pairing_error_cb(BluezException("Failed"))
    else:
      self.new_device.properties["Paired"] = True
      self.pairing_return_cb(self.new_device.path)
    
    if self.timeout:
      gobject.source_remove(self.timeout)
    self.pairing_error_cb = None
    self.pairing_return_cb = None
        
  def passkey_error(self, error):
    print "passkey_error"
    self.stopped = True
    if self.timeout:
      gobject.source_remove(self.timeout)
    if self.pairing_error_cb:
      self.pairing_error_cb (error)
    self.pairing_error_cb = None
    self.pairing_return_cb = None
    
  def passkey_timeout(self):
    print "passkey_timeout"
    if not self.pairing_error_cb:
      return

    self.pairing_error_cb(BluezException("Failed"))
    self.agent.Cancel(dbus_interface="org.bluez.Agent")
    self.RemoveDevice(self.new_device.path)
    self.pairing_error_cb = None
    self.pairing_return_cb = None

  @dbus.service.method(dbus_interface='org.bluez.Adapter',
                       in_signature='sos', out_signature='o',
                       sender_keyword='sender',
                       async_callbacks=("return_cb", "error_cb"))
  def CreatePairedDevice(self, address, agent_name, capability, return_cb, error_cb, sender):
    if self.error:
      raise BluezException(self.error)

    self.agent = dbus.SystemBus().get_object(sender, agent_name, introspect = False)
    new_device = None
    device_found = False

    for obj_path, device in self.devices.items():
      if device.properties["Address"] == address:
        if device.properties["Paired"] == True:
          raise BluezException("Failed")
        new_device = device
        device_found = True
        break

    if new_device is None:
      for device in self.devices_around:
        if device.properties["Address"] == address:
          new_device = BluezDevice(self.main_loop, self.path, 
                                   device.properties["Address"],
                                   device.properties)
          # TODO: add UUIDs
          self.devices[new_device.path] = new_device
          self.DeviceCreated(new_device.path)
          break
    
    if new_device is None:
      raise BluezException("Failed")

    self.pairing_return_cb = return_cb
    self.pairing_error_cb = error_cb

    self.new_device = new_device
    self.agent.RequestPinCode(dbus.ObjectPath(new_device.path),
                              dbus_interface="org.bluez.Agent",
                              reply_handler=self.pincode_reply,
                              error_handler=self.passkey_error)

    self.timeout = gobject.timeout_add(10000, self.passkey_timeout)

  @dbus.service.method(dbus_interface='org.bluez.Adapter',
                       in_signature='o', out_signature='')
  def RemoveDevice(self, device_path):
    if self.error:
      raise BluezException(self.error)

    if not self.devices.has_key(device_path):
      raise BluezException("Failed")

    del self.devices[device_path]
    self.DeviceRemoved(device_path)
    
  @dbus.service.method(dbus_interface='org.bluez.Adapter',
                       in_signature='os', out_signature='',
                       sender_keyword='sender')
  def RegisterAgent(self, agent_name, capability, sender = None):
    if self.error:
      raise BluezException(self.error)

    bus = dbus.SystemBus()
    self.global_agent = bus.get_object(sender, agent_name, introspect = False)

  @dbus.service.method(dbus_interface='org.bluez.Adapter',
                       in_signature='o', out_signature='')
  def UnregisterAgent(self, agent_name):
    if self.error:
      raise BluezException(self.error)

    #self.agents[agent_name].Release()
    del self.global_agent

  #
  # DBUS signals
  #

  @dbus.service.signal(dbus_interface='org.bluez.Adapter',
                       signature='sv')
  def PropertyChanged(self, name, value):
    print "adapter PropertyChanged (%s, %s) signal" % (str(name), str(value))

  @dbus.service.signal(dbus_interface='org.bluez.Adapter',
                       signature='sa{sv}')
  def DeviceFound(self, address, values):
    print "adapter DeviceFound signal"

  @dbus.service.signal(dbus_interface='org.bluez.Adapter',
                       signature='s')
  def DeviceDisappeared(self, address):
    print "adapter DeviceDisappeared signal"

  @dbus.service.signal(dbus_interface='org.bluez.Adapter',
                       signature='o')
  def DeviceCreated(self, device):
    print "adapter DeviceCreated signal"

  @dbus.service.signal(dbus_interface='org.bluez.Adapter',
                       signature='o')
  def DeviceRemoved(self, device):
    print "adapter DeviceRemoved signal"

  #
  # DBUS test methods
  #

  @dbus.service.method(dbus_interface='org.bluez.Adapter.Testing',
                       in_signature='ssuas', out_signature='')
  def AddDeviceAround(self, bt_address, name, cod, uuids):
    self.devices_around.append(BluetoothDevice(bt_address, name, cod, uuids))

  def null_reply(self, passkey = None):
    pass

  @dbus.service.method(dbus_interface='org.bluez.Adapter.Testing',
                       in_signature='s', out_signature='')
  def TestAgentRequest(self, what):
    if what not in ("PinCode", "Passkey", "Confirmation"):
      raise BluezException("WrongMethod")

    new_device = BluezDevice(self.main_loop, self.path, 
                             self.devices_around[0].properties["Address"],
                             self.devices_around[0].properties)
    self.devices[new_device.path] = new_device
    self.DeviceCreated(new_device.path)

    agent_obj = self.global_agent
    if what == "PinCode":
      agent_obj.RequestPinCode(dbus.ObjectPath(new_device.path),
                               dbus_interface="org.bluez.Agent",
                               reply_handler=self.null_reply,
                               error_handler=self.null_reply)
    elif what == "Passkey":
      agent_obj.RequestPasskey(dbus.ObjectPath(new_device.path),
                               dbus_interface="org.bluez.Agent",
                               reply_handler=self.null_reply, 
                               error_handler=self.null_reply)
    elif what == "Confirmation":
      agent_obj.RequestConfirmation(dbus.ObjectPath(new_device.path),
                                    dbus.UInt32(111111),
                                    dbus_interface="org.bluez.Agent",
                                    reply_handler=self.null_reply,
                                    error_handler=self.null_reply)

  @dbus.service.method(dbus_interface='org.bluez.Adapter.Testing',
                       in_signature='s', out_signature='')
  def SetError(self, error):
    self.error = error


# Manager stuff
class BluezManager(dbus.service.Object):

  adapters = []
  default_adapter = 0
  error = ""

  def __init__(self, main_loop):
    # Here the object path
    dbus.service.Object.__init__(self,
                                 dbus.service.BusName("org.bluez", bus=dbus.SystemBus()),
                                 "/")

    self.main_loop = main_loop
    self.stopped = False

  #
  # DBUS methods
  #

  @dbus.service.method(dbus_interface='org.bluez.Manager',
                       in_signature='', out_signature='o')
  def DefaultAdapter(self):
    if self.error:
      raise BluezException(self.error)

    if not self.adapters:
      raise BluezException("NoSuchAdapter")
    
    return (self.adapters[self.default_adapter])

  @dbus.service.method(dbus_interface='org.bluez.Manager',
                       in_signature='s', out_signature='o')
  def FindAdapter(self, pattern):
    if self.error:
      raise BluezException(self.error)

    adapter_idx = 0
    for adapter in self.adapters:
      if adapter.name.find(pattern) != -1:
        return (adapter)
      if adapter.properties["Address"].find(pattern) != -1:
        return (adapter)
    raise BluezException("NoSuchAdapter")

  @dbus.service.method(dbus_interface='org.bluez.Manager',
                       in_signature='', out_signature='ao')
  def ListAdapters(self):
    if self.error:
      raise BluezException(self.error)

    return (self.adapters)

  #
  # DBUS signals
  #

  @dbus.service.signal(dbus_interface='org.bluez.Manager',
                       signature='o')
  def AdapterAdded(self, adapter):
    print "manager AdapterAdded signal"

  @dbus.service.signal(dbus_interface='org.bluez.Manager',
                       signature='o')
  def AdapterRemoved(self, adapter):
    print "manager AdapterRemoved signal"

  @dbus.service.signal(dbus_interface='org.bluez.Manager',
                       signature='o')
  def DefaultAdapterChanged(self, adapter):
    print "manager DefaultAdapterChanged signal"

  #
  # DBUS test methods
  #

  @dbus.service.method(dbus_interface='org.bluez.Manager.Testing',
                       in_signature='s', out_signature='o')
  def AddAdapter(self, bt_address):
    new_path = "/org/bluez/hci%d" % (len(self.adapters))
    adapter = BluezAdapter(self.main_loop, new_path, bt_address)
    self.adapters.append(adapter)
    self.AdapterAdded(adapter)

    # Set default adapter, if this is the only one
    if len(self.adapters) == 1:
      self.default_adapter = 0
      self.DefaultAdapterChanged(self.adapters[self.default_adapter])
    return adapter

  @dbus.service.method(dbus_interface='org.bluez.Manager.Testing',
                       in_signature='o', out_signature='')
  def RemoveAdapter(self, adapter_name):
    for adapter in self.adapters:
      if adapter.name == adapter_name[1:]:
        self.AdapterRemoved(adapter)
        if self.default_adapter == self.adapters.index(adapter):
          self.default_adapter = 0
          if len(self.adapters) > 1:
            self.DefaultAdapterChanged(self.adapters[self.default_adapter])
        self.adapters.remove(adapter)
        return
    raise BluezException("NoSuchAdapter")

  @dbus.service.method(dbus_interface='org.bluez.Manager.Testing',
                       in_signature='o', out_signature='')
  def SetDefaultAdapter(self, adapter_name):
    for adapter in self.adapters:
      if adapter.name == adapter_name[1:]:
        self.default_adapter = self.adapters.index(adapter)
        self.DefaultAdapterChanged(self.adapters[self.default_adapter])
        return
    raise BluezException("NoSuchAdapter") 

  @dbus.service.method(dbus_interface='org.bluez.Manager.Testing',
                       in_signature='s', out_signature='')
  def SetError(self, error):
    self.error = error


# Main stuff
if __name__ == "__main__":
  DBusGMainLoop(set_as_default=True)

  # Here register the service name
  loop = gobject.MainLoop()
  manager_server = BluezManager(loop)
  loop.run()

