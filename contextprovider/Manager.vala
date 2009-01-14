// FIXME: Things to do:
// - Add mutual exclusivity

using GLib;
using DBus;

namespace ContextProvider {
	
	[DBus (name = "org.freedesktop.ContextKit.Manager")]
	public class Manager : GLib.Object /*, DBusManager */{
		int subscriber_count = 0;
		Gee.ArrayList<Subscriber> subscribers = new Gee.ArrayList<Subscriber>();
		
		Gee.HashMap<int, Subscriber> subscriber_addresses = new Gee.HashMap<int, Subscriber>(); // FIXME: This ok? weak?

		internal Manager() {
		
		
		}
		// FIXME: Return type! Two things to return...
		/*public HashTable<string, Variant?> Get (string[] keys) {
			// Implementation: Read values from the central value table.
			
			
			
			
			HashTable<string, Variant?> ret = new HashTable<string, Variant?> (str_hash,str_equal);
			todo, this is a bit fail, as we'll intern anything that comes off the wire,
			  leaving a possible DOS or at least random memory leaks 
			StringSet keyset = new StringSet.from_array (keys);
			foreach (var plugin in plugins) {
				plugin.Get (keyset, ret);
			}
			return ret;
		}*/

		public DBus.ObjectPath GetSubscriber (DBus.BusName name) throws DBus.Error {
			debug("Manager::GetSubscriber");
			
			//todo, check requesting bus name. the subscription object should be a singleton
			//for each bus name. FIXME: ?
			
			// FIXME: Add mutex

			// Create a subscription object
			var connection = DBus.Bus.get (DBus.BusType.SESSION);

			Subscriber s = new Subscriber(this, subscriber_count);
			subscriber_count++; // FIXME: Potential overflow? Do we need to worry?
			
			// Store information about this newly created subscription object
			subscribers.add (s);
			subscriber_addresses.set (dbus_address_to_int(name), s); // Track the dbus address
			
			connection.register_object ((string) s.object_path, s);
						
			// FIXME: Release mutex
			
			// Return the object path of the subscription object to the client
			return s.object_path;
	
		}
		
		/*
		Listen to subscribers exiting.
		*/
		internal void on_name_owner_changed (DBus.Object sender, string name, string old_owner, string new_owner) {
			//debug("Got a NameOwnerChanged signal");
			debug(name);
			debug(old_owner);
			debug(new_owner);
			
			int temp = dbus_address_to_int(name);
			if (subscriber_addresses.contains (temp) && new_owner == "") {
				debug ("Client died");
				
				// FIXME: Tell the corresponding subscriber to destroy itself
				subscriber_addresses.remove (temp);
			}
		}
		
		private int dbus_address_to_int(string name) {
			int64 result = 0;
			string name2 = name.replace(":", "");
			name2 = name2.replace(".", "");
			result = name2.to_int64();
			//debug ("name %s is int %d", name2, (int)result);
			return (int)result;
		}


	}
}
