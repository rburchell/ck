using GLib;
using Gee;

namespace ContextProvider {

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
			this.key_counter = key_counter;
			subscribed_keys = new StringSet();
		}

		~Subscriber () {
			debug ("Subscriber %s destroyed", object_path);
			// TODO: Unsubscribe all the keys currently subscribed to

			// Decrease the (global) subscriber count for the keys
			key_counter.remove (subscribed_keys);
		}

		// Emit the Changed signal over DBus
		internal void emit_changed (HashTable<string, Value?> values, ArrayList<string>? unavail_l) {
			string[] unavail = {};

			foreach (string str in unavail_l) {
				unavail += str;
			}

			Changed (values, unavail);
		}

		public void Subscribe (string[] keys, out HashTable<string, Value?> values, out string[] undeterminable_keys) {
			// Check input against valid keys
			StringSet keyset = manager.check_keys(keys);

			message ("Subscribe: requested valid keys %s", keyset.debug());

			// Ignore keys which are already subscribed to
			StringSet new_keys = new StringSet.difference (keyset, subscribed_keys);
			message ("Subscribe: new keys %s", new_keys.debug());

			// Increase the subscriber count of the new keys.
			key_counter.add (new_keys);

			subscribed_keys = new StringSet.union(subscribed_keys, new_keys);

			// Read the values from the central value table and return them
			// Note: Use also those keys which were already subscribed to (and are subscribed to again)
			manager.get_internal (keyset, out values, out undeterminable_keys);

			debug ("Subscribe done");
		}

		public void Unsubscribe (string[] keys) {
			// Check input against valid keys
			StringSet keyset = manager.check_keys(keys);

			// Ignore keys which are not subscribed to
			StringSet decreasing_keys = new StringSet.intersection (keyset, subscribed_keys);

			// Decrease the (global) subscriber count for the keys 
			key_counter.remove (decreasing_keys);

			// Track the keys which are currently subscribed to (with this subscriber) 
			subscribed_keys = new StringSet.difference (subscribed_keys, decreasing_keys);

			// FIXME: Other actions needed?
		}

		/* Is called when the value of a property changes */
		internal void on_value_changed(HashTable<string, Value?> changed_properties, ArrayList<string>? changed_undeterminable) {
			HashTable<string, Value?> values_to_send = new HashTable<string, Value?>(str_hash, str_equal);
			ArrayList<string> undeterminable_keys = new ArrayList<string>();

			// Loop through the properties we got and
			// check if the client is interested in them.
			var keys = changed_properties.get_keys ();
			// Note: get_keys returns a list of unowned strings. We shouldn't assign it to
			// a list of owned strings. At the moment, the Vala compiler doesn't prevent us
			// from doing so. 
			foreach (var key in keys) {
				if (subscribed_keys.is_member (key)) {
					// The client of this subscriber is interested in the key
					Value? v = changed_properties.lookup (key);

					if (v == null) { // FIXME: Is this needed?
						undeterminable_keys.add (key);
					}
					else {
						values_to_send.insert (key, v);
					}
				}
			}

			// Loop through the undetermined properties we got and
			// check if the client is interested in them.
			foreach (var key in changed_undeterminable) {
				if (subscribed_keys.is_member (key)) {
					// The client of this subscriber is interested in the key
					undeterminable_keys.add (key);
				}
			}

			// Check if we have something to send to the client
			if (values_to_send.size () > 0 || undeterminable_keys.size > 0) {
				emit_changed (values_to_send, undeterminable_keys);
			}
		}
	}
}
