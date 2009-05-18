/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

using Gee;

namespace ContextProvider {

	// Records whether the context_init function has been called
	internal Manager? manager = null;
	internal dynamic DBus.Object? bus = null;
	/**
	 * context_provider_set_integer:
	 * @key: name of key
	 * @value: new value of key
	 *
	 * Set a key to have an integer value of #value
	 */
	public void set_integer (string key, int value) {
		assert (manager != null);

		Value v = Value (typeof(int));
		v.set_int (value);
		manager.property_value_change (key, v);
	}

	/**
	 * context_provider_set_double:
	 * @key: name of key
	 * @value: new value of key
	 *
	 * Set a key to have an floating point value of #value
	 */
	public void set_double (string key, double value) {
		assert (manager != null);

		Value v = Value (typeof(double));
		v.set_double (value);
		manager.property_value_change (key, v);
	}

	/**
	 * context_provider_set_boolean:
	 * @key: name of key
	 * @value: new value of key
	 *
	 * Set a key to have an boolean value of #val
	 */
	public void set_boolean (string key, bool value) {
		assert (manager != null);

		Value v = Value (typeof(bool));
		v.set_boolean (value);
		manager.property_value_change (key, v);
	}

	/**
	 * context_provider_set_string:
	 * @key: name of key
	 * @value: new value of key
	 *
	 * Set a key to have an boolean value of #val
	 */
	public void set_string (string key, string value) {
		assert (manager != null);

		Value v = Value (typeof(string));
		v.set_string (value);
		manager.property_value_change (key, v);
	}

	/**
	 * context_provider_set_null:
	 * @key: name of key
	 *
	 * Marks #key as not having a determinable value.
	 */
	public void set_null (string key) {
		assert (manager != null);

		manager.property_value_change (key, null);
	}

	/**
	 * ContextProviderSubscriptionChangedCallback:
	 * @subscribe: TRUE if the group was subscribed to or FALSE if unsubscribed from
	 * @user_data: the user data passed in #context_provider_install
	 *
	 * Type definition for a function that will be called back when
	 * the first key or keys in a group become subscribed or when
	 * all the keys in the group have become unsubscribed.
	 */
	public delegate void SubscriptionChangedCallback(bool subscribe);

	private void log_null(string? log_domain, LogLevelFlags log_levels, string message) {
		/* nop */
	}


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
		// Don't emit debug() messages unless $CONTEXT_DEBUG is defined.
		if (GLib.Environment.get_variable("CONTEXT_DEBUG") == null)
			GLib.Log.set_handler("ContextKit", GLib.LogLevelFlags.LEVEL_DEBUG, log_null);
		try {
			var connection = DBus.Bus.get (bus_type);
			bus = connection.get_object ( "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
			if (bus_name != null) {
				// try to register service in session bus
				uint request_name_result = bus.RequestName (bus_name, (uint) 0);

				if (request_name_result != DBus.RequestNameReply.PRIMARY_OWNER) {
					debug ("Unable to register bus name '%s'", bus_name);
					return false;
				}
			}

			debug ("Creating new Manager D-Bus service");

			// Create the Manager object and register it over DBus.
			manager = new Manager (bus_type);
			bus.NameOwnerChanged += manager.on_name_owner_changed;

			debug ("Registering new Manager D-Bus service");
			connection.register_object ("/org/freedesktop/ContextKit/Manager", manager);
		} catch (DBus.Error e) {
			debug ("Registration failed: %s", e.message);
			manager = null;
			return false;
		}


		return true;
	}

	/**
	 * context_provider_stop:
	 *
	 * Stop providing context and free up all resources.
	 *
	 * Closes down ContextProvider and frees up all resources.
	 */
	public void stop () {
		manager = null;
		bus = null;
	}


	/**
	 * context_provider_install_group:
	 * @key_group: a NULL-terminated array of context key names
	 * @clear_values_on_subscribe: if TRUE then the keys in this group will
	 * be reset to null when the group is first subscribed to after being
	 * unsubscribed from.
	 * @subscription_changed_cb: a #ContextProviderSubscriptionChangedCallback to be called when the subscription status changes on this group
	 * @subscription_changed_cb_target: user data to pass to #subscription_changed_cb
	 * 
	 */
	public void install_group ([CCode (array_length = false, array_null_terminated = true)] string[] key_group, bool clear_values_on_subscribe, SubscriptionChangedCallback? subscription_changed_cb) {
		assert (manager != null);
		Group g = new Group(key_group, clear_values_on_subscribe, subscription_changed_cb);
		manager.group_list.add(g);
	}

	/**
	 * context_provider_install_key:
	 * @key: a context key name
	 * @clear_values_on_subscribe: if TRUE then this key will be reset to null
	 *	when first subscribed to after being unsubscribed from.
	 * @subscription_changed_cb: a #ContextProviderSubscriptionChangedCallback to be called when the subscription status changes on this key
	 * @subscription_changed_cb_target: user data to pass to #subscription_changed_cb
	 *
	 */
	public void install_key(string key, bool clear_values_on_subscribe, SubscriptionChangedCallback? subscription_changed_cb) {
		assert (manager != null);
		string[] key_group = {key};
		Group g = new Group(key_group, clear_values_on_subscribe, subscription_changed_cb);
		manager.group_list.add(g);
	}


}
