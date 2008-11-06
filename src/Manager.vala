using GLib;

namespace ContextKit {
	public class Manager : GLib.Object, IDBusManager {
		public HashTable<string, TypedVariant?> Get (string[] keys) {
			HashTable<string, TypedVariant?> ret = new HashTable<string, TypedVariant?> (str_hash,str_equal);
			TypedVariant tv;
			tv.type = ValueType.TRUTH;
			tv.value.init (typeof(bool));
			tv.value.set_boolean (true);
			ret.insert ("something",tv);
			/*foreach (var key in keys) {
				ret
			}*/
			return ret;
		}

		public DBus.ObjectPath Subscribe (string[] keys, out HashTable<string, TypedVariant?> values) {
			return new DBus.ObjectPath ("/test");
		}
	}
}
