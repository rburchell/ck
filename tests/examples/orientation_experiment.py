import traceback
import dbus
import sys
import time

def get_orientation():
	bus = dbus.SessionBus()
	edge_key = "Context.Device.Orientation.edgeUp"
	facing_key = "Context.Device.Orientation.facingUp"
	try:
		object  = bus.get_object("org.freedesktop.ContextKit","/org/freedesktop/ContextKit/Manager")
		iface = dbus.Interface(object,"org.freedesktop.ContextKit.Manager")
		ret = iface.Get([edge_key, facing_key])
		#print "The keys are:", ret.keys()
		
		struct1 = ret[edge_key]
		struct2 = ret[facing_key]
		
		print "Orientation:", struct1[1], struct2[1]
		
	except dbus.DBusException:
		print "Caught an exception"
		traceback.print_exc()

if __name__ == "__main__":
	while (True):
		print "Getting the orientation..."
		get_orientation()
		time.sleep(1)
	

