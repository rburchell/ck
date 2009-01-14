using GLib;

namespace ContextProvider {
	[DBus (name = "org.freedesktop.ContextKit.Subscriber")]
	public class Subscriber : GLib.Object {
		Manager manager;
		internal DBus.ObjectPath object_path {get; set;}
		//StringSet subscribed_keys;

		
		internal Subscriber (Manager manager, int id) {
			string path = "/org/freedesktop/ContextKit/Subscribers/%d".printf (id);
			this.object_path = new DBus.ObjectPath (path);
			this.manager = manager;
			//this.subscribed_keys = new StringSet();
		}
		
		// FIXME: API changes affect these
		/*
		internal void emit_changed (HashTable<string, TypedVariant?> values) {
			HashTable<string, ValueArray> new_values = new HashTable<string, ValueArray>(str_hash, str_equal);
			List <weak string> keys = values.get_keys();
			foreach (var k in keys) {
				weak TypedVariant? v = values.lookup (k);
				ValueArray va = new ValueArray(2);
				Value type = Value(typeof(int));
				type.set_int (v.type);
				va.append(type);
				Value value = Value(typeof(Value));
				value.set_boxed(&v.value);
				va.append(value);
				new_values.insert (k, #va);
			}
			Changed (new_values);
		}*/

		/*public HashTable<string, TypedVariant?> Subscribe (string[] keys) {
			HashTable<string, TypedVariant?> values = new HashTable<string, TypedVariant?> (str_hash,str_equal);
			StringSet keyset = new StringSet.from_array (keys);
			message ("Subscribe: requested %s", keyset.debug());
			StringSet new_keys = new StringSet.difference (keyset, subscribed_keys);
			message ("Subscribe: new keys %s", new_keys.debug());

			foreach (var plugin in manager.plugins) {
				plugin.Subscribe (new_keys, this);
				plugin.Get (keyset, values);
			}
			return values;
		}*/

		public void Unsubscribe (string[] keys) {
			debug("Subscriber::Unsubscribe");
		}
		/*public signal void Changed (HashTable<string,TypedVariant?> values);*/
	}
}
