namespace Context {

	public class ChangeSet {
		internal HashTable<string, Value?> properties;
		internal List<string> undeterminable_keys;

		internal ChangeSet () {
			properties = new HashTable<string, Value?>(str_hash, str_equal);
			undeterminable_keys = new List<string>();
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
		
		public int send_notification () {
			ContextKit.Manager manager = ContextKit.Manager.get_instance ();
			manager.property_values_changed(properties, undeterminable_keys);
			return 0;
		}
	}

	// Definitions of callback function types
	public delegate void get_callback(ContextKit.StringSet keys, void* values);
	// FIXME: use int (hashtable index) instead of void*
	public delegate void subscribe_callback(ContextKit.StringSet keys);

	public int init(string[] provided_keys, get_callback get_cb, subscribe_callback first_cb, subscribe_callback last_cb) {
		ContextKit.Manager manager = ContextKit.Manager.get_instance ();
		
		ContextKit.CProvider.Provider provider = new ContextKit.CProvider.Provider(get_cb, first_cb, last_cb);
		manager.providers.register (provided_keys, provider);
		return 1;
	}
	
	public int no_of_subscribers (string key) {
		ContextKit.Manager manager = ContextKit.Manager.get_instance ();
		return manager.key_counter.number_of_subscribers (key);
	}
}

