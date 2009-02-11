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
	public class ChangeSet {

		/**
		 * ContextProviderChangeSet:
		 *
		 * An opaque structure representing a set of key's values
		 */

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
		 * Returns: a new #ContextProviderChangeSet
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
		 * @change_set: a #ContextProviderChangeSet to commit
		 *
		 * Emits the contents of the changeset to all
		 * listeners interested in the properties that have changed.
		 *
		 * Returns: TRUE if changes were emitted
		 */
		public static bool commit (ChangeSet #change_set /*take back ownership */) {
			assert (initialised == true);

			ContextProvider.Manager manager = ContextProvider.Manager.get_instance ();
			bool ret = manager.property_values_changed(change_set.properties, change_set.undeterminable_keys);
//			changeset_holder_mutex.lock();
			//removing it from the changeset_holder causes refcount to go to zero.
			changeset_holder.remove (change_set);
//			changeset_holder_mutex.unlock();
			return ret;
		}

		/**
		 * context_provider_change_set_cancel:
		 * @change_set : a #ContextProviderChangeSet to cancel
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
		 * @self: a #ContextProviderChangeSet to add a value to
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
		 * @self: a #ContextProviderChangeSet to add a value to
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
		 * @self: a #ContextProviderChangeSet to add a value to
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
		 * context_provider_change_set_add_undetermined_key:
		 * @self: a #ContextProviderChangeSet to which to add an undeterminable value
		 * @key: name of key
		 *
		 * Marks #key as not having a determinable value.
		 */
		public void add_undetermined_key (string key) {
			assert (initialised == true);

			undeterminable_keys.prepend (key);
		}
	}

	/**
	 * ContextProviderGetCallback:
	 * @keys: a #ContextProviderStringSet of keys for which values have been requested
	 * @change_set: a #ContextProvideChangeSet in which to fill in the requested values
	 * @user_data: the user data passed in #context_provider_install
	 *
	 * Type definition for a function that will be called back when key values are needed.
	 * The keys will only ever be keys that this provider registered as providing.
	 */
	public delegate void GetCallback(StringSet #keys, ChangeSet #change_set);

	/**
	 * ContextProviderSubscribedCallback:
	 * @keys_subscribed: a #ContextProviderStringSet of keys which have been newly subscibed
	 * @user_data: the user data passed in #context_provider_install
	 *
	 * Type definition for a function that will be called back when a key is first subscribed to
	 * The keys will only ever be keys that this provider registered as providing.
	 */
	public delegate void SubscribedCallback(StringSet keys_subscribed);

	/**
	 * ContextProviderUnsubscribedCallback:
	 * @keys_unsubscribed: a #ContextProviderStringSet of keys that have been newly unsubscribed
	 * @keys_remaining: a #ContextProviderStringSet of keys provided by this provider that are still
	 * subscribed to
	 * @user_data: the user data passed in #context_provider_install
	 *
	 * Type definition for a function that will be called back when a key is last unsubscribed from.
	 * The keys will only ever be keys that this provider registered as providing.
	 */
	public delegate void UnsubscribedCallback(StringSet keys_unsubscribed, StringSet keys_remaining);

	/**
	 * context_provider_init:
	 * @bus_type: which bus to advertise context on
	 * @bus_name: A well-known bus name to register, or NULL if not required.
	 *
	 * Initialise the Context Provider library.
	 *
	 * This sets up the D-Bus connection and optionally requests a well known bus name.
	 * It must be called before using any other context provider functionality.
	 *
	 * Returns: TRUE if successful, FALSE otherwise.
	 */
	public bool init (DBus.BusType bus_type, string? bus_name) {

		try {
			if (bus_name != null) {
				var connection = DBus.Bus.get (bus_type);
				dynamic DBus.Object bus = connection.get_object ( "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
				// try to register service in session bus
				uint request_name_result = bus.RequestName (bus_name, (uint) 0);

				if (request_name_result != DBus.RequestNameReply.PRIMARY_OWNER) {
					debug ("Unable to register bus name '%s'", bus_name);
					return false;
				}
			}

			debug ("Creating new Manager D-Bus service");


			Manager.set_bus_type (bus_type);

			Manager manager = Manager.get_instance ();

			debug ("Registering new Manager D-Bus service");
			var connection = DBus.Bus.get (bus_type);

			connection.register_object ("/org/freedesktop/ContextKit/Manager", manager);
		} catch (DBus.Error e) {
			debug ("Registration failed: %s", e.message);
			return false;
		}


		initialised = true;
		return true;
	}

	/**
	 * context_provider_install:
	 * @provided_keys: an array of context key names
	 * @get_cb: a #ContextProviderGetCallback to be called when values are requested for keys
	 * @get_cb_target: use data to pass to #get_cb
	 * @subscribed_cb: a #SubscribedCallback to be called when a key is first subscribed to
	 * @subscribed_cb_target: use data to pass to #subscribed_cb
	 * @unsubscribed_cb: a #UnsubscribedCallback to be called when a key is last unsubscribed from
	 * @unsubscribed_cb_target: use data to pass to #unsubscribed_cb
	 *
	 * Install a new service providing context.
	 *
	 * Returns: a new #ContextProvider object
	 */
	public Provider install ([CCode (array_length = false, array_null_terminated = true)] string[] provided_keys, GetCallback? get_cb, SubscribedCallback? subscribed_cb, UnsubscribedCallback? unsubscribed_cb) {

		Manager manager = Manager.get_instance ();

		CProvider provider = new CProvider(new StringSet.from_array(provided_keys), get_cb, subscribed_cb, unsubscribed_cb);
		manager.providers.register (provider);
		return provider;
	}

	/**
	 * context_provider_remove:
	 * @provider: a #ContextProvder created with #context_provider_install
	 *
	 * Remove and dealloctate resources for a #ContextProvider created with #context_provider_install
	 */
	public void remove (Provider provider) {
		Manager manager = Manager.get_instance ();
		manager.providers.unregister (provider);
	}

	/**
	 * context_provider_no_of_subscribers:
	 * @key: the name of a context key
	 *
	 * Query the current number of subscribers for a given context key
	 */
	public int no_of_subscribers (string key) {
		Manager manager = Manager.get_instance ();
		return manager.key_counter.number_of_subscribers (key);
	}
}
