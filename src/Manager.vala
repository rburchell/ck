using GLib;

namespace ContextKit {

	/*TODO: use an interface so all the methods aren't bus-accessible...*/
	[DBus (name = "org.freedesktop.ContextKit.Subscriber")]
	public class Subscriber : GLib.Object {
		Manager manager;
		internal DBus.ObjectPath object_path {get; set;}
		StringSet subscribed_keys;

		internal Subscriber (Manager manager, int id) {
			string path = "/org/freedesktop/ContextKit/Subscribers/%d".printf (id);
			this.object_path = new DBus.ObjectPath (path);
			this.manager = manager;
			this.subscribed_keys = new StringSet();
		}

		internal void emit_changed (HashTable<string, Value?> values, List<string> unavail_l) {
			string[] unavail = {};

			foreach (string str in unavail_l) {
				unavail += str;
			}

			Changed (values, unavail);
		}

		public HashTable<string, Value?> Subscribe (string[] keys, out string[] unavailable_keys) {
			HashTable<string, Value?> values = new HashTable<string, Value?> (str_hash,str_equal);
			StringSet keyset = new StringSet.from_array (keys);
			message ("Subscribe: requested %s", keyset.debug());
			StringSet new_keys = new StringSet.difference (keyset, subscribed_keys);
			message ("Subscribe: new keys %s", new_keys.debug());

			List<string> unavail_l = new List<string>();
			foreach (var plugin in manager.plugins) {
				plugin.Subscribe (new_keys, this);
				plugin.Get (keyset, values, unavail_l);
			}

			string [] unavail = {};
			foreach (string str in unavail_l) {
				unavail += str;
			}

			unavailable_keys = unavail;
			return values;
		}

		public void Unsubscribe (string[] keys) {
		}
		public signal void Changed (HashTable<string, Value?> values, string[] unavailable);
	}

	public class Manager : GLib.Object, DBusManager {
		int subscriber_count = 0;
		Gee.ArrayList<Subscriber> subscribers = new Gee.ArrayList<Subscriber>();
		// Mapping client dbus names into subscription objects related to those clients.
		Gee.HashMap<string, Subscriber> subscriber_addresses = new Gee.HashMap<string, Subscriber>(str_hash, str_equal); // FIXME: This ok? weak?
		internal Gee.ArrayList<Plugin> plugins;

		public Manager() {
			plugins = new Gee.ArrayList<Plugin>();
			plugins.add(new MCE.Plugin());
			//plugins.add(new Location2.Plugin());
		}

		public void Get (string[] keys, out HashTable<string, Value?> values, out string[] undeterminable_keys) {
			values = new HashTable<string, Value?> (str_hash,str_equal);
			/*todo, this is a bit fail, as we'll intern anything that comes off the wire,
			  leaving a possible DOS or at least random memory leaks */
			StringSet keyset = new StringSet.from_array (keys);
			List<string> unavail_key_list = new List<string>();

			foreach (var plugin in plugins) {
				plugin.Get (keyset, values, unavail_key_list);
			}

			string[] unavail = {};
			foreach (string str in unavail_key_list) {
				unavail += str;
			}
			undeterminable_keys = unavail;
		}

		public DBus.ObjectPath GetSubscriber (DBus.BusName name) throws DBus.Error {
			//todo, check requesting bus name. the subscription object should be a singleton
			//for each bus name.

			var connection = DBus.Bus.get (DBus.BusType.SESSION);

			// FIXME: Mutual exclusivity?
			
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

				// FIXME: Tell the corresponding subscriber to destroy itself
				subscriber_addresses.remove (name);
			}
		}

	}
}
