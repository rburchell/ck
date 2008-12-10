import traceback
import dbus
import sys
import gobject

from dbus.mainloop.glib import DBusGMainLoop

edge_key = "Context.Device.Orientation.edgeUp"
facing_key = "Context.Device.Orientation.facingUp"

def handle_signal(changed):
	print "Got signal"
	struct1 = changed[edge_key]
	struct2 = changed[facing_key]
	print "Orientation:", struct1[1], struct2[1]

def execute_subscribe():
	bus = dbus.SessionBus()
	try:
		proxy_object_manager = bus.get_object("org.freedesktop.ContextKit","/org/freedesktop/ContextKit/Manager")
		iface_manager = dbus.Interface(proxy_object_manager, "org.freedesktop.ContextKit.Manager")
		print "Requesting the subscriber object"
		# Get the subscriber object
		new_object_path = iface_manager.GetSubscriber()
		
		print "Got an object path", new_object_path
		# And then execute the actual subscribe property_names
		
		proxy_object_subscriber = bus.get_object("org.freedesktop.ContextKit", new_object_path)
		iface_subscriber = dbus.Interface(proxy_object_subscriber, "org.freedesktop.ContextKit.Subscriber")
		
		# Connect to the signal
		print "Connecting to the signal"
		iface_subscriber.connect_to_signal("Changed", handle_signal)
		
		print "Executing the Subscribe"
		ret = iface_subscriber.Subscribe([edge_key, facing_key])
		
		print "Returned:", ret
		
		loop = gobject.MainLoop()
		loop.run()

	except dbus.DBusException:
		print "Caught an exception"
		traceback.print_exc()


if __name__ == "__main__":
	DBusGMainLoop(set_as_default=True)
	
	execute_subscribe()



