using GLib;

namespace ContextKit {

	public class Providers {
		// List of providers
		Gee.ArrayList<Plugin> providers = new Gee.ArrayList<Plugin> ();

		// Valid keys provided by all providers
		public StringSet valid_keys = new StringSet();

		public void register_provider(string[] keys, Plugin provider) {
			providers.add (provider);
			valid_keys = new StringSet.union (valid_keys, new StringSet.from_array (keys));
		}

		public List<string> get (StringSet keys, HashTable<string, Value?> values) {
			List<string> unavail = new List<string> ();
			foreach (var provider in providers) {
				provider.Get (keys, values, ref unavail);
			}
			return unavail;
		}

		public void first_subscribed(StringSet keys) {
			foreach (var provider in providers) {
				provider.KeysSubscribed (keys);
			}
		}

		public void last_unsubscribed(StringSet keys) {
			foreach (var provider in providers) {
				provider.KeysUnsubscribed (keys);
			}
		}


	}

	public class KeyUsageCounter {
		// The number of subscribers for each key (over all subscriber objects)
		Gee.HashMap<string, int> no_of_subscribers = new Gee.HashMap<string, int>(str_hash, str_equal);
		Providers providers;

		public KeyUsageCounter (Providers providers) {
			this.providers = providers;
		}

		public void add (StringSet keys) {
			StringSet first_subscribed_keys = new StringSet();

			foreach (var key in keys) {
				if (no_of_subscribers.contains (key)) {
					int old_value = no_of_subscribers.get (key);
					no_of_subscribers.set (key, old_value + 1);

					if (old_value == 0) {
						// This is the first subscribed to the key
						first_subscribed_keys.add (key);
					}

					debug ("Subscriber count of %s is now %d", key, old_value + 1);
				}
				else { // Key name not present in the key table
					no_of_subscribers.set (key, 1);

					first_subscribed_keys.add (key);
					debug ("Subscriber count of %s is now %d", key, 1);
				}
			}

			if (first_subscribed_keys.size() > 0) {
				// Signal that some new keys were subscribed to
				providers.first_subscribed (first_subscribed_keys);
			}

			// FIXME: Do we need to care about which keys are subscribed to? What if the keys are something not provided by this provider?
		}

		public void remove (StringSet keys) {
			StringSet last_unsubscribed_keys = new StringSet();

			foreach (var key in keys) {
				if (no_of_subscribers.contains (key)) {
					int old_value = no_of_subscribers.get (key);

					if (old_value >= 1) {
						// Do not store negative values
						no_of_subscribers.set (key, old_value - 1);
					}

					if (old_value <= 1) {
						// The last subscriber for this key unsubscribed
						last_unsubscribed_keys.add (key);
					}
					debug ("Subscriber count of %s is now %d", key, old_value - 1);
				}
				else {
					// This should not happen
					debug ("Error: decreasing the subscriber count of an unknown key.");
				}
			}

			if (last_unsubscribed_keys.size() > 0) {
				// Signal that some keys are not subscribed to anymore
				providers.last_unsubscribed (last_unsubscribed_keys);
			}
		}

}

	public class Subscriber : GLib.Object, DBusSubscriber {
		static Manager manager;
		internal DBus.ObjectPath object_path {get; set;}

		// Keys subscribed to by this subscriber
		StringSet subscribed_keys;
		KeyUsageCounter key_counter;

		internal Subscriber (Manager manager, KeyUsageCounter key_counter, int id) {
			string path = "/org/freedesktop/ContextKit/Subscribers/%d".printf (id);
			this.object_path = new DBus.ObjectPath (path);
			this.manager = manager;
			subscribed_keys = new StringSet();
		}

		~Subscriber () {
			debug ("Subscriber %s destroyed", object_path);
			// TODO: Unsubscribe all the keys currently subscribed to

			// Decrease the (global) subscriber count for the keys
			key_counter.remove (subscribed_keys);
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
			/*** TODO - check input against valid keys ****/
			StringSet keyset = new StringSet.from_array (keys);
			message ("Subscribe: requested %s", keyset.debug());

			// Ignore keys which are already subscribed to
			StringSet new_keys = new StringSet.difference (keyset, subscribed_keys);
			message ("Subscribe: new keys %s", new_keys.debug());

			// Increase the subscriber count of the new keys.
			key_counter.add (new_keys);

			subscribed_keys = new StringSet.union(subscribed_keys, new_keys);

			// Read the values from the central value table and return them
			manager.Get (keys, out values, out undeterminable_keys);

			debug ("Subscribe done");
		}

		public void Unsubscribe (string[] keys) {
			/*** TODO - check input against valid keys ****/
			// Ignore keys which are not subscribed to
			StringSet keyset = new StringSet.from_array (keys);
			StringSet decreasing_keys = new StringSet.intersection (keyset, subscribed_keys);

			// Decrease the (global) subscriber count for the keys 
			key_counter.remove (decreasing_keys);

			// Track the keys which are currently subscribed to (with this subscriber) 
			subscribed_keys = new StringSet.difference (subscribed_keys, decreasing_keys);

			// FIXME: Other actions needed?
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

		Providers providers;
		KeyUsageCounter key_counter;

		// NULL value means undetermined
		static HashTable<string, Value?> values = new HashTable<string, Value?>(str_hash, str_equal);


		public Manager(Providers providers) {
			this.providers = providers;
		}

		public void Get (string[] keys, out HashTable<string, Value?> values_to_send, out string[] undeterminable_keys) {
			debug ("Manager.Get");

			/*todo, this is a bit fail, as we'll intern anything that comes off the wire,
			  leaving a possible DOS or at least random memory leaks */
			HashTable<string, Value?> values = new HashTable<string, Value?> (str_hash, str_equal);
			StringSet keyset = new StringSet.from_array (keys);
			List<string> undeterminable_key_list = providers.get (keyset, values);

			// Then read the values from the value table
			insert_to_value_table (values, undeterminable_key_list);
			read_from_value_table(keys, out values_to_send, out undeterminable_keys);
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
			Subscriber s = new Subscriber(this, key_counter, subscriber_count);
			subscriber_count++; //keep a count rather than use subscribers.length for immutability
			// FIXME: Potential overflow? Do we need to worry?

			// Store information about this newly created subscription object
			subscribers.add (s);
			subscriber_addresses.set (name, s); // Track the dbus address

			// Return the object path of the subscription object to the client
			connection.register_object ((string) s.object_path, s);

			return s.object_path;
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
		public void insert_to_value_table(HashTable<string, Value?> properties, List<string>? undeterminable_keys) {
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
			insert_to_value_table(properties, null);

			// Inform the subscribers of the change
			foreach (var s in subscribers) {
				s.on_value_changed(properties);
			}
		}

	}
}
