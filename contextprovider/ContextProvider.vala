using GLib;

namespace ContextProvider {

	public class LibraryMain : Object {
		public LibraryMain () {
		}

		private static ContextProvider.Manager manager;
		
		// DBus data
		private dynamic DBus.Object bus;

		/* 
		ContextProvider library API function
		Start providing the ContextKit DBus interface.
		*/
		public bool start() {
			try {
				var connection = DBus.Bus.get (DBus.BusType.SESSION);
				bus = connection.get_object ( "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
				// Try to register service in session bus
				uint request_name_result = bus.RequestName ("org.freedesktop.ContextKit", (uint) 0);

				// FIXME: Provider name should be a parameter
				
				if (request_name_result == DBus.RequestNameReply.PRIMARY_OWNER) {
					debug ("Creating new Manager D-Bus service");
					manager = new ContextProvider.Manager();
					connection.register_object ("/org/freedesktop/ContextKit/Manager", manager);
					
					// Make manager listen to the NameOwnerChanged
					bus.NameOwnerChanged += manager.on_name_owner_changed;
					// FIXME: each signal is got twice. why?
					
				} else {
					debug ("Manager D-Bus service is already running");
					return false;
				}
				
				


			} catch (Error e) {
				error ("Error connecting to dbus: %s", e.message); 
				return false;
			}
			return true;
		}
		
		/* 
		ContextProvider library API function
		Called by the provider to signal that values of some context properties have changed.
		*/
		public static bool values_changed(/*Some kind of list of properties + values*/) {
			return false; // Not implemented
		}
		
		
	}
}
