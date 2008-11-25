using GLib;

namespace ContextKit {

	[DBus (name = "org.freedesktop.ContextKit.Manager")]
	public class Manager : GLib.Object /*, DBusManager */{
		MCE.Plugin mce_plugin = new MCE.Plugin();

		public Manager() {
		}

		public HashTable<string, TypedVariant?> Get (string[] keys) {
			HashTable<string, TypedVariant?> ret = new HashTable<string, TypedVariant?> (str_hash,str_equal);
			/*todo, this is a bit fail, as we'll intern anything that comes off the wire,
			  leaving a possible DOS or at least random memory leaks */
			StringSet keyset = new StringSet.from_array (keys);
			mce_plugin.Get (keyset, ret);
			return ret;
		}

		public DBus.ObjectPath Subscribe (string[] keys, out HashTable<string, TypedVariant?> values) {
			return new DBus.ObjectPath ("/test");
		}
	}
}
