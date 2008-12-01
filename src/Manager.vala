using GLib;

namespace ContextKit {


	/*TODO: use an interface so alk the methods aren't bus-accessible...*/
	[DBus (name = "org.freedesktop.ContextKit.Subscription")]
	public class Subscription : GLib.Object {
		Subscription (int id) {
			string path = "/org/freedesktop/ContextKit/subscriptions/%d".printf (id);
			this.object_path = new DBus.ObjectPath (path);
		}

		public DBus.ObjectPath object_path {get; construct;}
		public void Unsubscribe () {
		}
		public signal void Changed (HashTable<string, TypedVariant?> values);
	}

	[DBus (name = "org.freedesktop.ContextKit.Manager")]
	public class Manager : GLib.Object /*, DBusManager */{
		int subscribtion_count = 0;
		Gee.ArrayList<Plugin> plugins;
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

		public DBus.ObjectPath Subscribe (string[] keys, out HashTable<string, TypedVariant?> values) {
			values = new HashTable<string, TypedVariant?> (str_hash,str_equal);
			StringSet keyset = new StringSet.from_array (keys);
			Subscription subscription = new Subscription(subscribtion_count++);
			foreach (var plugin in plugins) {
				plugin.Subscribe (keyset, subscription);
				plugin.Get (keyset, values);
			}
			return subscription.object_path;
		}
	}
}
