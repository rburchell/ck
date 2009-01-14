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

		internal void emit_changed (HashTable<string, Value> values) {
			Changed (values);
		}

		public HashTable<string, Value> Subscribe (string[] keys, out string[] unavailable_keys) {
			HashTable<string, Value> values = new HashTable<string, Value> (str_hash,str_equal);
			StringSet keyset = new StringSet.from_array (keys);
			message ("Subscribe: requested %s", keyset.debug());
			StringSet new_keys = new StringSet.difference (keyset, subscribed_keys);
			message ("Subscribe: new keys %s", new_keys.debug());

			foreach (var plugin in manager.plugins) {
				plugin.Subscribe (new_keys, this);
				plugin.Get (keyset, values, unavailable_keys);
			}
			return values;
		}

		public void Unsubscribe (string[] keys) {
		}
		public signal void Changed (HashTable<string, Value> values);
	}

	[DBus (name = "org.freedesktop.ContextKit.Manager")]
	public class Manager : GLib.Object /*, DBusManager */{
		int subscriber_count = 0;
		Gee.ArrayList<Subscriber> subscribers = new Gee.ArrayList<Subscriber>();
		internal Gee.ArrayList<Plugin> plugins;

		public Manager() {
			plugins = new Gee.ArrayList<Plugin>();
			plugins.add(new MCE.Plugin());
		}

		public HashTable<string, Value> Get (string[] keys, out string[] unavailable_keys) {
			HashTable<string, Value > ret = new HashTable<string, Value> (str_hash,str_equal);
			/*todo, this is a bit fail, as we'll intern anything that comes off the wire,
			  leaving a possible DOS or at least random memory leaks */
			StringSet keyset = new StringSet.from_array (keys);

			foreach (var plugin in plugins) {
				plugin.Get (keyset, ret, unavailable_keys);
			}
			return ret;
		}

		public DBus.ObjectPath GetSubscriber () throws DBus.Error {
			//todo, check requesting bus name. the subscription object should be a singleton
			//for each bus name.

			var connection = DBus.Bus.get (DBus.BusType.SESSION);

			Subscriber s = new Subscriber(this, subscriber_count);
			subscriber_count++; //keep a count rather than use subscribers.length for immutability
			subscribers.add (s);

			connection.register_object ((string) s.object_path, s);
			return s.object_path;
		}
	}
}
