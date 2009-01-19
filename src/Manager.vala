using GLib;

namespace ContextKit {

	// Declaration of the callback function used to signal subscription changes to the provider
	public delegate void subscribe_callback(string[] keys);
	public delegate void get_callback(string[] keys, ref HashTable<string, Value?> values, ref string[] undeterminable_keys); // FIXME: out, ref, ???

	public class Subscriber : GLib.Object, DBusSubscriber {
		static Manager manager;
		internal DBus.ObjectPath object_path {get; set;}

		// Keys subscribed to by this subscriber
		StringSet subscribed_keys;

		// The number of subscribers for each key (over all subscriber objects)
		static Gee.HashMap<string, int> no_of_subscribers = new Gee.HashMap<string, int>(str_hash, str_equal);

		internal Subscriber (Manager manager, int id) {
			string path = "/org/freedesktop/ContextKit/Subscribers/%d".printf (id);
			this.object_path = new DBus.ObjectPath (path);
			this.manager = manager;
			subscribed_keys = new StringSet();
		}

		~Subscriber () {
			debug ("Subscriber %s destroying itself", object_path);

			// Unsubscribe all the keys currently subscribed to

			// Decrease the (global) subscriber count for the keys
			decrease_subscriber_count (subscribed_keys.to_array());

			subscribed_keys.clear();

		}

		// Emit the Changed signal over DBus
		internal void emit_changed (HashTable<string, Value?> values, List<string> unavail_l) {
			string[] unavail = {};

			foreach (string str in unavail_l) {
				unavail += str;
			}

			Changed (values, unavail);
		}

		public void Subscribe (string[] keys, out HashTable<string, Value?> values, out string[] undeterminable_keys) {
			StringSet keyset = new StringSet.from_array (keys);
			message ("Subscribe: requested %s", keyset.debug());

			// Ignore keys which are already subscribed to
			StringSet new_keys = new StringSet.difference (keyset, subscribed_keys);
			message ("Subscribe: new keys %s", new_keys.debug());

			// Ignore keys which are not recognized as valid keys
			new_keys = new StringSet.intersection (new_keys, manager.valid_keys);

			// Increase the subscriber count of the new keys.
			increase_subscriber_count(new_keys.to_array());

			subscribed_keys = new StringSet.union(subscribed_keys, new_keys);

			// Read the values from the central value table and return them
			manager.Get (keys, out values, out undeterminable_keys);

			// Here is the old plugin-type implementation
			/*values = new HashTable<string, Value?> (str_hash,str_equal);
			List<string> unavail_l = new List<string>();
			foreach (var plugin in manager.plugins) {
				plugin.Subscribe (new_keys, this);
				plugin.Get (keyset, values, unavail_l);
			}

			string [] unavail = {};
			foreach (string str in unavail_l) {
				unavail += str;
			}

			undeterminable_keys = unavail;*/
			//debug ("Subscribe done");
		}

		public void Unsubscribe (string[] keys) {
			// Ignore keys which are not subscribed to
			StringSet keyset = new StringSet.from_array (keys);
			StringSet decreasing_keys = new StringSet.intersection (keyset, subscribed_keys);

			// Ignore keys which are not recognized as valid keys
			decreasing_keys = new StringSet.intersection (decreasing_keys, manager.valid_keys);
			
			// Decrease the (global) subscriber count for the keys 
			decrease_subscriber_count (decreasing_keys.to_array());
			
			// Track the keys which are currently subscribed to (with this subscriber) 
			subscribed_keys = new StringSet.difference (subscribed_keys, decreasing_keys);

			// FIXME: Other actions needed?
		}

		// Record the subscriber count for each key.
		private static void increase_subscriber_count(Gee.ArrayList<string> keys) {
			string[] first_subscribed_keys = {};
			foreach (var key in keys) {
				if (no_of_subscribers.contains (key)) {
					int old_value = no_of_subscribers.get (key);
					no_of_subscribers.set (key, old_value + 1);

					if (old_value == 0) {
						// This is the first subscribed to the key
						first_subscribed_keys += key;
					}

					debug ("Subscriber count of %s is now %d", key, old_value + 1);
				}
				else { // Key name not present in the key table
					no_of_subscribers.set (key, 1);

					first_subscribed_keys += key;
					debug ("Subscriber count of %s is now %d", key, 1);
				}
			}

			if (first_subscribed_keys.length > 0) {
				// Signal that some new keys were subscribed to
				manager.first_subscribed (first_subscribed_keys);
			}

			// FIXME: Do we need to care about which keys are subscribed to? What if the keys are something not provided by this provider?
		}

		private static void decrease_subscriber_count(Gee.ArrayList<string> keys) {
			string[] last_unsubscribed_keys = {};
			foreach (var key in keys) {
				if (no_of_subscribers.contains (key)) {
					int old_value = no_of_subscribers.get (key);

					if (old_value >= 1) {
						// Do not store negative values
						no_of_subscribers.set (key, old_value - 1);
					}

					if (old_value <= 1) {
						// The last subscriber for this key unsubscribed
						last_unsubscribed_keys += key;
					}
					debug ("Subscriber count of %s is now %d", key, old_value - 1);
				}
				else {
					// This should not happen
					debug ("Error: decreasing the subscriber count of an unknown key.");
				}
			}

			if (last_unsubscribed_keys.length > 0) {
				// Signal that some keys are not subscribed to anymore
				manager.last_unsubscribed (last_unsubscribed_keys);
			}
		}

		/* Is called when the value of a property changes */
		internal void on_value_changed(HashTable<string, Value?> properties) {
			HashTable<string, Value?> values_to_send = new HashTable<string, Value?>(str_hash, str_equal);
			GLib.List<string> undeterminable_keys = new GLib.List<string>();

			GLib.List<string> keys = properties.get_keys ();
			foreach (var key in keys) {
				if (subscribed_keys.is_member (key)) {
					// The client of this subscriber is interested in the key
					Value? v = properties.lookup (key);

					if (v == null) {
						undeterminable_keys.append (key);
					}
					else {
						values_to_send.insert (key, v);
					}
				}
			}

			// Check if we have something to send to the client
			if (values_to_send.size () > 0 || undeterminable_keys.length () > 0) {
				emit_changed (values_to_send, undeterminable_keys);
			}
		}
	}

	public class Manager : GLib.Object, DBusManager {
		int subscriber_count = 0;

		// Stored subscriber objects
		Gee.HashSet<Subscriber> subscribers = new Gee.HashSet<Subscriber>();

		// Mapping client dbus names into subscription objects

		Gee.HashMap<string, Subscriber> subscriber_addresses = new Gee.HashMap<string, Subscriber>(str_hash, str_equal);

		// The value table holding the values of the keys
		// NULL value means undetermined
		static HashTable<string, Value?> values = new HashTable<string, Value?>(str_hash, str_equal);

		// Stored pointers to plugin callback functions
		private Gee.ArrayList<PluginStruct?> plugin_pointers;

		// All valid keys provided by the plugins owned by this Manager
		public StringSet valid_keys; 

		internal Gee.ArrayList<Plugin> plugins;

		public Manager() {
			plugins = new Gee.ArrayList<Plugin>();
			plugins.add(new MCE.Plugin());
			//plugins.add(new Location2.Plugin());
			
			
			plugin_pointers = new Gee.ArrayList<PluginStruct?>();
			valid_keys = new StringSet();
		}

		public void Get (string[] keys, out HashTable<string, Value?> values_to_send, out string[] undeterminable_keys) {
			debug ("Manager.Get");
			//values_to_send = new HashTable<string, Value?> (str_hash,str_equal);
			//undeterminable_keys = {};

			/*todo, this is a bit fail, as we'll intern anything that comes off the wire,
			  leaving a possible DOS or at least random memory leaks */

			// FIXME: Note: this is a draft of the new functionality

			
			// Give the providers / plugins an opportunity to update the values
			foreach (var plugin in plugin_pointers) {
				HashTable<string, Value?> values_temp = new HashTable<string, Value?>(str_hash, str_equal);
				string[] undeterminable_keys_temp = {};
				// Give the plugin an opportunity to update the Get'ted values
				// ... and other values, if wanted
				debug("Calling the cb function");
				plugin.get_cb (keys, ref values_temp, ref undeterminable_keys_temp);
				// Update the value table
				insert_to_value_table (values_temp, undeterminable_keys_temp);
			}

			// Then read the values from the value table
			read_from_value_table(keys, out values_to_send, out undeterminable_keys);
			

			// Here is the old, plugin-type functionality:
/*
			StringSet keyset = new StringSet.from_array (keys);
			List<string> unavail_key_list = new List<string>();

			foreach (var plugin in plugins) {
				plugin.Get (keyset, values, unavail_key_list);
			}

			string[] unavail = {};
			foreach (string str in unavail_key_list) {
				unavail += str;
			}
			undeterminable_keys = unavail;*/
		}

		public DBus.ObjectPath GetSubscriber (DBus.BusName name) throws DBus.Error {

			/* First check if the same client has already requested a subscriber object.
			If so, return its object path. */

			if (subscriber_addresses. contains(name)) {
				Subscriber s = subscriber_addresses.get (name);
				return s.object_path;
			}

			/* Else, create a new subscriber and return its object path.
			*/

			var connection = DBus.Bus.get (DBus.BusType.SESSION);

			// Create a subscription object
			Subscriber s = new Subscriber(this, subscriber_count); 
			subscriber_count++; //keep a count rather than use subscribers.length for immutability
			// FIXME: Potential overflow? Do we need to worry?

			// Store information about this newly created subscription object
			subscribers.add (s);
			subscriber_addresses.set (name, s); // Track the dbus address

			// Return the object path of the subscription object to the client
			connection.register_object ((string) s.object_path, s);

			return s.object_path;
		}

		internal void first_subscribed(string[] keys) {
			foreach (var plugin in plugin_pointers) {
				plugin.first_subscribed_cb (keys);
			}
		}
		
		internal void last_unsubscribed(string[] keys) {
			foreach (var plugin in plugin_pointers) {
				plugin.last_unsubscribed_cb (keys);
			}
		}

		/*
		Listen to subscribers exiting.
		*/
		internal void on_name_owner_changed (DBus.Object sender, string name, string old_owner, string new_owner) {
			debug("Got a NameOwnerChanged signal");
			debug(name);
			debug(old_owner);
			debug(new_owner);

			if (subscriber_addresses.contains (name) && new_owner == "") {
				debug ("Client died");

				// Remove the subscriber
				Subscriber s = subscriber_addresses.get (name);
				subscribers.remove(s);

				subscriber_addresses.remove (name);

				// Now nobody holds a pointer to the subscriber so it is destroyed automatically.
			}
		}

		/*
		Update the value table with new values.
		*/
		public void insert_to_value_table(HashTable<string, Value?> properties, string[] undeterminable_keys) {
			//debug ("insert_to_value_table");
			GLib.List<string> keys = properties.get_keys ();
			foreach (var key in keys) {
				// Overwrite the value in the value table.
				// Do not care whether it was already there or not.
				values.insert (key, properties.lookup (key));
			}
			foreach (var key in undeterminable_keys) {
				values.insert (key, null);
			}
		}

		/*
		Read the values of the specified keys from the value table.
		*/
		public void read_from_value_table(string[] keys, out HashTable<string, Value?> properties, out string[] undeterminable_keys) {
			debug ("read_from_value_table");
			// Note: Vala doesn't support += for parameters yet; using a temp array
			properties = new HashTable<string, Value?>(str_hash, str_equal);
			string[] undeterminable_keys_temp = {}; 
			foreach (var key in keys) {
				//debug ("reading value for key %s", key);
				Value? v = values.lookup (key);

				if (v == null) {
					undeterminable_keys_temp += key;
				}
				else {
					properties.insert (key, v);
				}
			}
			undeterminable_keys = undeterminable_keys_temp;
			debug ("read_from_value_table done");
		}

		/* Is called when the provider sets new values to context properties */
		public void property_values_changed(HashTable<string, Value?> properties) {
			// Update the value table
			insert_to_value_table(properties, {});

			// Inform the subscribers of the change
			foreach (var s in subscribers) {
				s.on_value_changed(properties);
			}
		}
		
		public void register_plugin(string[] keys, PluginStruct plugin_struct) {
			plugin_pointers.add (plugin_struct);
			valid_keys = new StringSet.union (valid_keys, new StringSet.from_array (keys));
		}
	}
}
