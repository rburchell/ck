using Gee;

namespace ContextProvider {

	public class ChangeSet {
		HashTable<string, Value?> properties;
		/*shouldn't be public, but lists suck, see cprovider.vala*/
		public GLib.List<string> undeterminable_keys;

		/* TODO. some proper thinking needs to be done to make this
		   whole lib thread safe
		   */
//		static private Mutex changeset_holder_mutex;
		private static HashSet<ChangeSet> changeset_holder = new HashSet<ChangeSet>();

		/**
		 * context_provider_change_set_create:
		 *
		 * Create a new change set to emit key changes
		 * Returns: a new #ChangeSet
		 */
		public static ChangeSet create () {
			ChangeSet s = new ChangeSet();
//			changeset_holder_mutex.lock();
			changeset_holder.add(s);
//			changeset_holder_mutex.unlock();
			// s will have a floating reference
			return s;
		}

		/**
		 * context_provider_change_set_commit:
		 * @change_set: a #ChangeSet to commit
		 *
		 * Emits the contents of the changeset to all
		 * listeners interested in the properties that have changed.
		 */
		public static void commit (ChangeSet #change_set /*take back ownership */) {
			ContextProvider.Manager manager = ContextProvider.Manager.get_instance ();
			manager.property_values_changed(change_set.properties, change_set.undeterminable_keys);
//			changeset_holder_mutex.lock();
			//removing it from the changeset_holder causes refcount to go to zero.
			changeset_holder.remove (change_set);
//			changeset_holder_mutex.unlock();
		}

		/**
		 * context_provider_change_set_cancel:
		 * @change_set: a #ChangeSet to cancel
		 *
		 * Cancels a changeset, cleaning up without emitting the contents.
		 */
		public static void cancel (ChangeSet #change_set /*take back ownership */) {
//			changeset_holder_mutex.lock();
			//removing it from the changeset_holder causes refcount to go to zero.
			changeset_holder.remove (change_set);
//			changeset_holder_mutex.unlock();
		}


		private ChangeSet () {
			properties = new HashTable<string, Value?>(str_hash, str_equal);
			undeterminable_keys = new GLib.List<string>();
		}

		internal ChangeSet.from_get (HashTable<string, Value?> properties)
		{
			this.properties = properties;
			this.undeterminable_keys = new GLib.List<string>();
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

	// Definitions of callback function types
	public delegate void GetCallback(StringSet #keys, ChangeSet #change_set);
	public delegate void SubscribeCallback(StringSet keys);

	public bool init ([CCode (array_length = false, array_null_terminated = true)] string[] provided_keys, GetCallback? get_cb, SubscribeCallback? first_cb, SubscribeCallback? last_cb) {
		Manager manager = Manager.get_instance ();

		CProvider provider = new CProvider(get_cb, first_cb, last_cb);
		manager.providers.register (provided_keys, provider);

		try {
			debug ("Registering new Manager D-Bus service");
			var connection = DBus.Bus.get (DBus.BusType.SESSION);

			connection.register_object ("/org/freedesktop/ContextKit/Manager", manager);
		} catch (DBus.Error e) {
			debug ("Registration failed: %s", e.message);
			return false;
		}
		return true;
	}

	public int no_of_subscribers (string key) {
		Manager manager = Manager.get_instance ();
		return manager.key_counter.number_of_subscribers (key);
	}
}
