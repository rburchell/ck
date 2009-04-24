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

	internal class Manager : GLib.Object, DBusManager {
		// Mapping client dbus names into subscription objects
		public Gee.HashMap<string, Subscriber> subscribers {get; private set;}

		public GroupList group_list {get; private set;}
		public KeyUsageCounter key_counter {get; private set;}

		// NULL value means undetermined
		HashTable<string, Value?> values;
		dynamic DBus.Object bus; // Needs to be stored so that we get the NameOwnerChanged

		// Session / system bus option
		DBus.BusType busType;

		int subscriber_count = 0;

		public Manager(DBus.BusType b = DBus.BusType.SESSION) throws DBus.Error {
			busType = b;
			/*TODO, should manager own the key counter? */
			group_list = new GroupList();
			key_counter = new KeyUsageCounter();

			key_counter.keys_added += group_list.first_subscribed;
			key_counter.keys_removed += group_list.last_unsubscribed;
			
			subscribers = new Gee.HashMap<string, Subscriber>(str_hash, str_equal);
			values = new HashTable<string, Value?>(str_hash, str_equal);

			// Start listening to the NameOwnerChanged signal (of the correct bus)
			// to know when the clients exit.
		}

		public void get_internal (StringSet keyset, out HashTable<string, Value?> properties, out string[] undeterminable_keys) {
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
		public void on_name_owner_changed (DBus.Object? sender, string name, string old_owner, string new_owner) {
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
		public StringSet check_keys(string[] keys) {
			// Do not create a StringSet from the parameter "keys" as that would be add Quarks
			StringSet checked_keys = new StringSet();
			foreach (var key in keys) {
				if (group_list.valid_keys.is_member(key)) {
					checked_keys.add(key);
				}
			}
			return checked_keys;
		}

		/* Is called when the group sets new values to context properties */
		public void property_value_change(string key, Value? value) {
			if (group_list.valid_keys.is_member (key) == false) {
				warning ("Key %s is not valid", key);
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
