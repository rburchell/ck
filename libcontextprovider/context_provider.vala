using Gee;

namespace ContextProvider {

	// Records whether the context_init function has been called
	bool initialised = false;

	/**
	 * SECTION:change_set
	 * @short_description: Context value change handling
	 * @include: context_provider.h
	 * @see_also: #ContextProviderGetCallback
	 *
	 * #ContextProviderChangeSet is used to signal changes to contextual values by a provider.
	 * As a provider you can asynchronously emit value changes for keys which you are providing
	 * by creating a #ContextProviderChangeSet and commiting it using #context_provider_change_set_commit.
	 * #ContextProviderChangeSet can also be provided to you, for example in
	 * #ContextProviderGetCallback. In this case, you should simply add the values requested
	 * and return without calling #context_provider_change_set_commit.
	 */

	/**
	 * ContextKitChangeSet
	 * An opaque structure representing a set of key's values
	 */
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
			assert (initialised == true);

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
			assert (initialised == true);

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
			assert (initialised == true);

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

		/**
		 * context_provider_change_set_add_integer:
		 * @change_set: a #ChangeSet to cancel
		 * @key: name of key
		 * @value: new value of key
		 *
		 * Set a key to have an integer value of #value
		 */
		public void add_integer (string key, int value) {
			assert (initialised == true);

			Value v = Value (typeof(int));
			v.set_int (value);
			properties.insert (key, v);
		}

		/**
		 * context_provider_change_set_add_double:
		 * @change_set: a #ChangeSet to cancel
		 * @key: name of key
		 * @value: new value of key
		 *
		 * Set a key to have an floating point value of #value
		 */
		public void add_double (string key, double value) {
			assert (initialised == true);

			Value v = Value (typeof(double));
			v.set_double (value);
			properties.insert (key, v);
		}

		/**
		 * context_provider_change_set_add_boolean:
		 * @change_set: a #ChangeSet to cancel
		 * @key: name of key
		 * @value: new value of key
		 *
		 * Set a key to have an boolean value of #val
		 */
		public void add_boolean (string key, bool value) {
			assert (initialised == true);

			Value v = Value (typeof(bool));
			v.set_boolean (value);
			properties.insert (key, v);
		}

		/**
		 * context_provider_change_set_add_undetermined key:
		 * @change_set: a #ChangeSet to cancel
		 * @key: name of key
		 *
		 * Marks #key as not having a determinable value.
		 */
		public void add_undetermined_key (string key) {
			assert (initialised == true);

			undeterminable_keys.prepend (key);
		}
	}

	// Definitions of callback function types
	public delegate void GetCallback(StringSet #keys, ChangeSet #change_set);
	public delegate void SubscribedCallback(StringSet keys_subscribed);
	public delegate void UnsubscribedCallback(StringSet keys_unsubscribed, StringSet keys_remaining);

	public bool init (bool useSessionBus, string? bus_name) {

		DBus.BusType busType = DBus.BusType.SESSION;
		if (useSessionBus == false) {
			debug ("Using system bus");
			busType = DBus.BusType.SYSTEM;
		}

		try {
			if (bus_name != null) {
				var connection = DBus.Bus.get (DBus.BusType.SESSION);
				dynamic DBus.Object bus = connection.get_object ( "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
				// try to register service in session bus
				uint request_name_result = bus.RequestName (bus_name, (uint) 0);

				if (request_name_result != DBus.RequestNameReply.PRIMARY_OWNER) {
					debug ("Unable to register bus name '%s'", bus_name);
					return false;
				}
			}

			debug ("Creating new Manager D-Bus service");


			Manager.setBusType (busType);

			Manager manager = Manager.get_instance ();

			debug ("Registering new Manager D-Bus service");
			var connection = DBus.Bus.get (busType);

			connection.register_object ("/org/freedesktop/ContextKit/Manager", manager);
		} catch (DBus.Error e) {
			debug ("Registration failed: %s", e.message);
			return false;
		}


		initialised = true;
		return true;
	}

	public Provider install ([CCode (array_length = false, array_null_terminated = true)] string[] provided_keys, GetCallback? get_cb, SubscribedCallback? first_cb, UnsubscribedCallback? last_cb) {

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
