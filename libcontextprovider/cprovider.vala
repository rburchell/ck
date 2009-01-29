using GLib;
using Gee;

namespace ContextProvider {

	public class ChangeSet {
		internal HashTable<string, Value?> properties;
		internal GLib.List<string> undeterminable_keys;

		/* TODO. some proper thinking needs to be done to make this
		   whole lib thread safe
		   */
//		static private Mutex changeset_holder_mutex;
		private static HashSet<ChangeSet> changeset_holder;

		public static ChangeSet create () {
			ChangeSet s = new ChangeSet();
//			changeset_holder_mutex.lock();
			changeset_holder.add(s);
//			changeset_holder_mutex.unlock();
			return #s;
		}

		public static void commit (ChangeSet #s) {
			ContextProvider.Manager manager = ContextProvider.Manager.get_instance ();
			manager.property_values_changed(s.properties, s.undeterminable_keys);
//			changeset_holder_mutex.lock();
			changeset_holder.remove (s);
//			changeset_holder_mutex.unlock();
		}

		internal ChangeSet () {
			properties = new HashTable<string, Value?>(str_hash, str_equal);
			undeterminable_keys = new GLib.List<string>();
		}

		public int add_int (string key, int val) {
			Value v = Value (typeof(int));
			v.set_int (val);
			properties.insert (key, v);
			return 0;
		}

		public int add_double (string key, double val) {
			Value v = Value (typeof(double));
			v.set_double (val);
			properties.insert (key, v);
			return 0;
		}

		public int add_undetermined_key (string key) {
			undeterminable_keys.prepend (key);
			return 0;
		}
	}

	internal class CProvider : GLib.Object, ContextProvider.Provider {

		// Stored callbacks
		private GetCallback get_cb;
		private SubscribeCallback first_cb;
		private SubscribeCallback last_cb;

		public CProvider (GetCallback get_cb, SubscribeCallback first_cb, SubscribeCallback last_cb) {
			this.get_cb = get_cb;
			this.first_cb = first_cb;
			this.last_cb = last_cb;
		}

		// Implementation of the Provider interface by using callbacks

		public void Get (StringSet keys, HashTable<string, Value?> ret, ref GLib.List<string> unavail) {
			ChangeSet change_set = new ChangeSet();
			get_cb (keys, change_set);
		}

		public void KeysSubscribed (StringSet keys) {
			first_cb (keys);
		}

		public void KeysUnsubscribed (StringSet keys) {
			last_cb (keys);
		}
	}
}
