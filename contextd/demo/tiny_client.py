#!/usr/bin/python

import traceback
import dbus
import sys
import gobject

from dbus.mainloop.glib import DBusGMainLoop

def printProperties(changed, undet):
	print "-------- New properties --------"
	print "Properties:"
	for key in changed.keys():
		struct1 = changed[key]
		print key, ":", struct1
	print "Undetermined:"
	for key in undet:
		print key
	print "--------------------------------"


def handleSignal(changed, undet):
	printProperties(changed, undet)

def execute_subscribe(property_names):
	bus = dbus.SessionBus()
	try:
		proxy_object_manager = bus.get_object("org.freedesktop.ContextKit.contextd","/org/freedesktop/ContextKit/Manager")
		iface_manager = dbus.Interface(proxy_object_manager, "org.freedesktop.ContextKit.Manager")

		#print "Requesting the subscriber object"
		# Get the subscriber object
		new_object_path = iface_manager.GetSubscriber()



		#print "Got an object path", new_object_path
		# And then execute the actual subscribe property_names

		proxy_object_subscriber = bus.get_object("org.freedesktop.ContextKit.contextd", new_object_path)
		iface_subscriber = dbus.Interface(proxy_object_subscriber, "org.freedesktop.ContextKit.Subscriber")

		# Connect to the signal
		#print "Connecting to the signal"
		iface_subscriber.connect_to_signal("Changed", handleSignal)

		#print "Executing the Subscribe"
		ret = iface_subscriber.Subscribe(property_names)

		printProperties(ret[0], ret[1])

		loop = gobject.MainLoop()
		loop.run()

	except dbus.DBusException:
		print "Caught an exception"
		traceback.print_exc()


if __name__ == "__main__":
	DBusGMainLoop(set_as_default=True)

	# You can write the properties of intrest directly here
	#execute_get(["Context.Device.Orientation.facingUp", "Context.Environment.Location.latitude", "Context.Environment.Location.longitude", "Context.Environment.Location.altitude"])

	# Or use the command line parameters
	print "Executing Subscribe with the properties", sys.argv[1:]
	execute_subscribe(sys.argv[1:])
