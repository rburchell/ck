using Gee;

namespace ContextProvider {

	// Records whether the context_init function has been called
	bool init_called = false;

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
			assert (init_called == true);

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
		public static int commit (ChangeSet #change_set /*take back ownership */) {
			assert (init_called == true);

			ContextProvider.Manager manager = ContextProvider.Manager.get_instance ();
			int ret = manager.property_values_changed(change_set.properties, change_set.undeterminable_keys);
//			changeset_holder_mutex.lock();
			//removing it from the changeset_holder causes refcount to go to zero.
			changeset_holder.remove (change_set);
//			changeset_holder_mutex.unlock();
			return ret; // Return the error / success value
		}

		/**
		 * context_provider_change_set_cancel:
		 * @change_set: a #ChangeSet to cancel
		 *
		 * Cancels a changeset, cleaning up without emitting the contents.
		 */
		public static void cancel (ChangeSet #change_set /*take back ownership */) {
			assert (init_called == true);

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
			assert (init_called == true);

			Value v = Value (typeof(int));
			v.set_int (val);
			properties.insert (key, v);
			return 0;
		}

		public int add_double (string key, double val) {
			assert (init_called == true);

			Value v = Value (typeof(double));
			v.set_double (val);
			properties.insert (key, v);
			return 0;
		}

		public int add_bool (string key, bool val) {
			assert (init_called == true);

			Value v = Value (typeof(bool));
			v.set_boolean (val);
			properties.insert (key, v);
			return 0;
		}

		public int add_undetermined_key (string key) {
			assert (init_called == true);

			undeterminable_keys.prepend (key);
			return 0;
		}
	}

	// Definitions of callback function types
	public delegate void GetCallback(StringSet #keys, ChangeSet #change_set);
	public delegate void SubscribeCallback(StringSet keys);

	public bool init (bool useSessionBus) {
		init_called = true;

		DBus.BusType busType = DBus.BusType.SESSION;
		if (useSessionBus == false) {
			debug ("Using system bus");
			busType = DBus.BusType.SYSTEM;
		}
		Manager.setBusType (busType);

		Manager manager = Manager.get_instance ();

		try {
			debug ("Registering new Manager D-Bus service");
			var connection = DBus.Bus.get (busType);

			connection.register_object ("/org/freedesktop/ContextKit/Manager", manager);
		} catch (DBus.Error e) {
			debug ("Registration failed: %s", e.message);
			return false;
		}
		return true;
	}

	public Provider install ([CCode (array_length = false, array_null_terminated = true)] string[] provided_keys, GetCallback? get_cb, SubscribeCallback? first_cb, SubscribeCallback? last_cb) {

		Manager manager = Manager.get_instance ();

		CProvider provider = new CProvider(new StringSet.from_array(provided_keys), get_cb, first_cb, last_cb);
		manager.providers.register (provider);
		return provider;
	}

	public void remove (Provider provider) {
		Manager manager = Manager.get_instance ();
		manager.providers.unregister (provider);
	}

	public int no_of_subscribers (string key) {
		Manager manager = Manager.get_instance ();
		return manager.key_counter.number_of_subscribers (key);
	}
}
