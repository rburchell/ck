using GLib;

namespace ContextProvider {

	public class Manager : GLib.Object, DBusManager {
		// Mapping client dbus names into subscription objects
		Gee.HashMap<string, Subscriber> subscribers;

		public Providers providers {get; private set;}
		public KeyUsageCounter key_counter {get; private set;}

		// NULL value means undetermined
		HashTable<string, Value?> values;
		static dynamic DBus.Object bus; // Needs to be stored so that we get the NameOwnerChanged

		int subscriber_count = 0;

		// Session / system bus option
		static DBus.BusType busType = DBus.BusType.SESSION;

		// Singleton implementation
		private static Manager? instance;
		public static Manager? get_instance() {
			if (instance == null) {
				instance = new Manager();
			}
			return instance;
		}

		private Manager() {
			/*TODO, should manager own the key counter? */
			this.providers = new Providers();
			this.key_counter = new KeyUsageCounter();
			this.subscribers = new Gee.HashMap<string, Subscriber>(str_hash, str_equal);
			this.values = new HashTable<string, Value?>(str_hash, str_equal);
			// Note: Use session / system bus according to the configuration
			// (which can be changed via setBusType).
			var connection = DBus.Bus.get (busType);
			bus = connection.get_object ( "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
			bus.NameOwnerChanged += this.on_name_owner_changed;
		}

		public static void setBusType(DBus.BusType b) {
			busType = b;
		}

		public void Get (string[] keys, out HashTable<string, Value?> values_to_send, out string[] undeterminable_keys) {
			debug ("Manager.Get");

			// Check input against valid keys
			// Do not create a StringSet from the parameter "keys" as that would be add Quarks
			StringSet keyset = check_keys(keys);
			get_internal (keyset, out values_to_send, out undeterminable_keys);
		}

		internal void get_internal (StringSet keyset, out HashTable<string, Value?> values_to_send, out string[] undeterminable_keys) {
			HashTable<string, Value?> values = new HashTable<string, Value?> (str_hash, str_equal);
			/*todo, this is a bit fail, as we'll intern anything that comes off the wire,
			  leaving a possible DOS or at least random memory leaks */

			// Ignore keys which are not recognized as valid
			StringSet valid_keyset = new StringSet.intersection (keyset, providers.valid_keys);

			// Let providers update the central value table
			List<string> undeterminable_key_list = providers.get (valid_keyset, values);
			insert_to_value_table (values, undeterminable_key_list);

			// Then read the values from the value table
			read_from_value_table(valid_keyset, out values_to_send, out undeterminable_keys);
		}

		public DBus.ObjectPath GetSubscriber (DBus.BusName name) throws DBus.Error {

			/* First check if the same client has already requested a subscriber object.
			If so, return its object path. */

			if (subscribers.contains(name)) {
				Subscriber s = subscribers.get (name);
				return s.object_path;
			}

			/* Else, create a new subscriber and return its object path.
			*/

			// Note: Use session / system bus according to the configuration
			// (which can be changed via setBusType).
			var connection = DBus.Bus.get (busType);

			// Create a subscription object
			Subscriber s = new Subscriber(this, key_counter, subscriber_count);
			subscriber_count++; //keep a count rather than use subscribers.length for immutability
			// FIXME: Potential overflow? Do we need to worry?

			// Store information about this newly created subscription object
			subscribers.set (name, s); // Track the dbus address

			// Return the object path of the subscription object to the client
			connection.register_object ((string) s.object_path, s);

			return s.object_path;
		}
		/*
		Listen to subscribers exiting.
		*/
		private void on_name_owner_changed (DBus.Object sender, string name, string old_owner, string new_owner) {
			debug("Got a NameOwnerChanged signal");
			debug(name);
			debug(old_owner);
			debug(new_owner);

			if (subscribers.contains (name) && new_owner == "") {
				debug ("Client died");

				// Remove the subscriber
				subscribers.remove (name);

				// Now nobody holds a pointer to the subscriber so it is destroyed automatically.
			}
		}

		/*
		Checks which keys are valid and returns them.
		*/
		internal StringSet check_keys(string[] keys) {
			// Do not create a StringSet from the parameter "keys" as that would be add Quarks
			StringSet checked_keys = new StringSet();
			foreach (var key in keys) {
				if (providers.valid_keys.is_member(key)) {
					checked_keys.add(key);
				}
			}
			return checked_keys;
		}

		/*
		Update the value table with new values.
		*/
		private void insert_to_value_table(HashTable<string, Value?> properties, List<string>? undeterminable_keys) {
			//debug ("insert_to_value_table");
			var keys = properties.get_keys ();
			// Note: get_keys returns a list of unowned strings. We shouldn't assign it to
			// a list of owned strings. At the moment, the Vala compiler doesn't prevent us
			// from doing so.
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
		private void read_from_value_table(StringSet keys, out HashTable<string, Value?> properties, out string[] undeterminable_keys) {
			debug ("read_from_value_table");
			// Note: Vala doesn't support += for parameters yet; using a temp array
			properties = new HashTable<string, Value?>(str_hash, str_equal);
			string[] undeterminable_keys_temp = {};
			foreach (var key in keys) {
				debug ("  %s", key);
				//debug ("reading value for key %s", key);
				Value? v = values.lookup (key);

				if (v == null) {
					debug ("    - undeterminable");
					undeterminable_keys_temp += key;
				}
				else {
					debug ("    - %s", v.strdup_contents());
					properties.insert (key, v);
				}
			}
			undeterminable_keys = undeterminable_keys_temp;
			debug ("read_from_value_table done");
		}

		/* Is called when the provider sets new values to context properties */
		public int property_values_changed(HashTable<string, Value?> properties, List<string>? undeterminable_keys) {
			// Check that all the keys are valid
			var keys = properties.get_keys ();
			foreach (var key in keys) {
				if (providers.valid_keys.is_member (key) == false) {
					error ("Key %s is not valid", key);
					// FIXME: How to react?
					// Now we drop the whole event and return an error value
					return 1;
				}
			}
			foreach (var key in undeterminable_keys) {
				if (providers.valid_keys.is_member (key) == false) {
					error ("Key %s is not valid", key);
					// FIXME: How to react?
					// Now we drop the whole event and return an error value
					return 1;
				}
			}

			// Update the value table
			insert_to_value_table(properties, undeterminable_keys);

			// Inform the subscribers of the change
			foreach (var s in subscribers.get_values()) {
				s.on_value_changed(properties, undeterminable_keys);
			}
			// Return success value
			return 0;
		}
	}
}
