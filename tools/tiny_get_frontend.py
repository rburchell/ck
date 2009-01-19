import traceback
import dbus
import sys

def execute_get(property_names):
	bus = dbus.SessionBus()
	try:
		object  = bus.get_object("org.freedesktop.ContextKit","/org/freedesktop/ContextKit/Manager")
		iface = dbus.Interface(object,"org.freedesktop.ContextKit.Manager")
		
		ret = iface.Get(property_names)
		print "Returned:", ret
		
		properties = ret[0]
		undetermined = ret[1]
		
		print "Properties and their values:"
		for name in properties.keys(): 
			print "Property:", name, ", value:", properties[name]
		
		print "Undetermined properties:"
		print undetermined
	except dbus.DBusException:
		print "Caught an exception"
		traceback.print_exc()

if __name__ == "__main__":
	# You can write the properties of intrest directly here
	#execute_get(["Context.Device.Orientation.displayFacingUp", "Context.Environment.Location.latitude", "Context.Environment.Location.longitude", "Context.Environment.Location.altitude"])
	
	# Or use the parameters from command line
	print "Executing Get with the properties", sys.argv[1:]
	execute_get(sys.argv[1:])

