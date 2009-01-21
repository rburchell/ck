using GLib;

namespace ContextKit {

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
		internal void emit_changed (HashTable<string, Value?> values, List<string>? unavail_l) {
			// Note: The unavail_l is never going to be NULL but it can be an empty list.
			// An empty GList is NULL and Vala doesn't know about that feature.
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
		internal void on_value_changed(HashTable<string, Value?> changed_properties, List<string>? changed_undeterminable) {
			HashTable<string, Value?> values_to_send = new HashTable<string, Value?>(str_hash, str_equal);
			List<string> undeterminable_keys = new GLib.List<string>();

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
						undeterminable_keys.append (key);
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
					undeterminable_keys.append (key);
				}
			}

			// Check if we have something to send to the client
			if (values_to_send.size () > 0 || undeterminable_keys.length () > 0) {
				emit_changed (values_to_send, undeterminable_keys);
			}
		}
	}
}
