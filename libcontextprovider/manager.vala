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

using GLib;
using Gee;

namespace ContextProvider {

	public class Manager : GLib.Object, DBusManager {
		// Mapping client dbus names into subscription objects
		Gee.HashMap<string, Subscriber> subscribers;

		public Groups groups {get; private set;}
		public KeyUsageCounter key_counter {get; private set;}

		// NULL value means undetermined
		HashTable<string, Value?> values;
		static dynamic DBus.Object bus; // Needs to be stored so that we get the NameOwnerChanged

		int subscriber_count = 0;

		// Session / system bus option
		static DBus.BusType busType = DBus.BusType.SESSION;

		// Singleton implementation
		private static Manager? instance;
		public static Manager? get_instance() {
			if (instance == null) {
				instance = new Manager();
			}
			return instance;
		}

		private Manager() {
			/*TODO, should manager own the key counter? */
			this.groups = new Groups();
			this.key_counter = new KeyUsageCounter();
			this.subscribers = new Gee.HashMap<string, Subscriber>(str_hash, str_equal);
			this.values = new HashTable<string, Value?>(str_hash, str_equal);
			// Note: Use session / system bus according to the configuration
			// (which can be changed via set_bus_type).
			var connection = DBus.Bus.get (busType);
			bus = connection.get_object ( "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
			bus.NameOwnerChanged += this.on_name_owner_changed;
		}

		public static void set_bus_type(DBus.BusType b) {
			busType = b;
		}

		internal void get_internal (StringSet keyset, out HashTable<string, Value?> properties, out string[] undeterminable_keys) {
			// Note: Vala doesn't support += for parameters yet; using a temp array
			properties = new HashTable<string, Value?>(str_hash, str_equal);
			string[] undeterminable_keys_temp = {};
			foreach (var key in keyset) {
				debug ("  %s", key);
				//debug ("reading value for key %s", key);
				Value? v = values.lookup (key);

				if (v == null) {
					debug ("    - undeterminable");
					undeterminable_keys_temp += key;
				}
				else {
					debug ("    - %s", v.strdup_contents());
					properties.insert (key, v);
				}
			}
			undeterminable_keys = undeterminable_keys_temp;
			debug ("read_from_value_table done");
		}

		public DBus.ObjectPath GetSubscriber (DBus.BusName name) throws DBus.Error {

			/* First check if the same client has already requested a subscriber object.
			If so, return its object path. */

			if (subscribers.contains(name)) {
				Subscriber s = subscribers.get (name);
				return s.object_path;
			}

			/* Else, create a new subscriber and return its object path.
			*/

			// Note: Use session / system bus according to the configuration
			// (which can be changed via setBusType).
			var connection = DBus.Bus.get (busType);

			// Create a subscription object
			Subscriber s = new Subscriber(this, key_counter, subscriber_count);
			subscriber_count++; //keep a count rather than use subscribers.length for immutability
			// FIXME: Potential overflow? Do we need to worry?

			// Store information about this newly created subscription object
			subscribers.set (name, s); // Track the dbus address

			// Return the object path of the subscription object to the client
			connection.register_object ((string) s.object_path, s);

			return s.object_path;
		}
		/*
		Listen to subscribers exiting.
		*/
		private void on_name_owner_changed (DBus.Object sender, string name, string old_owner, string new_owner) {
			debug("Got a NameOwnerChanged signal");
			debug(name);
			debug(old_owner);
			debug(new_owner);

			if (subscribers.contains (name) && new_owner == "") {
				debug ("Client died");

				// Remove the subscriber
				subscribers.remove (name);

				// Now nobody holds a pointer to the subscriber so it is destroyed automatically.
			}
		}

		/*
		Checks which keys are valid and returns them.
		*/
		internal StringSet check_keys(string[] keys) {
			// Do not create a StringSet from the parameter "keys" as that would be add Quarks
			StringSet checked_keys = new StringSet();
			foreach (var key in keys) {
				if (groups.valid_keys.is_member(key)) {
					checked_keys.add(key);
				}
			}
			return checked_keys;
		}

		/* Is called when the group sets new values to context properties */
		public void property_value_change(string key, Value? value) {
			if (groups.valid_keys.is_member (key) == false) {
				critical ("Key %s is not valid", key);
			}
			// ignore unchanged keys
			if (value_compare(values.lookup(key), value)) {
				return;
			}
			values.insert (key, value);

			// Inform the subscribers of the change
			foreach (var s in subscribers.get_values()) {
				s.on_value_changed(key, value);
			}
		}
	}
}
