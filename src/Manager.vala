using GLib;

namespace ContextKit {

	/*TODO: use an interface so all the methods aren't bus-accessible...*/
	[DBus (name = "org.freedesktop.ContextKit.Subscriber")]
	public class Subscriber : GLib.Object {
		Manager manager;
		internal DBus.ObjectPath object_path {get; set;}
		StringSet subscribed_keys;

		Subscriber (Manager manager, int id) {
			string path = "/org/freedesktop/ContextKit/subscribers/%d".printf (id);
			this.object_path = new DBus.ObjectPath (path);
			this.manager = manager;
			this.subscribed_keys = new StringSet();
		}

		public HashTable<string, TypedVariant?> Subscribe (string[] keys) {
			HashTable<string, TypedVariant?> values = new HashTable<string, TypedVariant?> (str_hash,str_equal);
			StringSet keyset = new StringSet.from_array (keys);
			StringSet new_keys = new StringSet.difference (keyset, subscribed_keys);

			foreach (var plugin in manager.plugins) {
				plugin.Subscribe (new_keys, this);
				plugin.Get (keyset, values);
			}
			return values;
		}

		public void Unsubscribe (string[] keys) {
		}
		public signal void Changed (HashTable<string, TypedVariant?> values);
	}

	[DBus (name = "org.freedesktop.ContextKit.Manager")]
	public class Manager : GLib.Object /*, DBusManager */{
		int subscriber_count = 0;
		internal Gee.ArrayList<Plugin> plugins;

		public Manager() {
			plugins = new Gee.ArrayList<Plugin>();
			plugins.add(new MCE.Plugin());
		}

		public HashTable<string, TypedVariant?> Get (string[] keys) {
			HashTable<string, TypedVariant?> ret = new HashTable<string, TypedVariant?> (str_hash,str_equal);
			/*todo, this is a bit fail, as we'll intern anything that comes off the wire,
			  leaving a possible DOS or at least random memory leaks */
			StringSet keyset = new StringSet.from_array (keys);
			foreach (var plugin in plugins) {
				plugin.Get (keyset, ret);
			}
			return ret;
		}

		public DBus.ObjectPath GetSubscriber () throws DBus.Error {
			//todo, check requesting bus name. the subscription object should be a singleton
			//for each bus name.

			var conn = DBus.Bus.get (DBus.BusType.SESSION);

			Subscriber s = new Subscriber(this, subscriber_count);
			subscriber_count++;

			conn.register_object ((string) s.object_path, s);
			return s.object_path;
		}
	}
}
