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

	public class Subscriber : GLib.Object, DBusSubscriber {
		Manager manager;
		public DBus.ObjectPath object_path {get; set;}

		// Keys subscribed to by this subscriber
		StringSet subscribed_keys;
		KeyUsageCounter key_counter;
		HashTable<string, Value?> values_to_send;
		bool idle_scheduled = false;

		public Subscriber (Manager manager, KeyUsageCounter key_counter, int id) {
			string path = "/org/freedesktop/ContextKit/Subscribers/%d".printf (id);
			this.object_path = new DBus.ObjectPath (path);
			this.manager = manager;
			this.key_counter = key_counter;
			subscribed_keys = new StringSet();
			this.values_to_send = new HashTable<string, Value?>(str_hash, str_equal);
		}

		~Subscriber () {
			debug ("Subscriber %s destroyed", object_path);
			// TODO: Unsubscribe all the keys currently subscribed to

			// Decrease the (global) subscriber count for the keys
			key_counter.remove (subscribed_keys);
		}

		// Emit the Changed signal over DBus
		public bool emit_changed () {
			string[] unavail = {};

			foreach (var i in values_to_send.get_keys()) {
				if (values_to_send.lookup(i) == null) {
					unavail += i;
					values_to_send.remove(i);
				}
			}

			Changed (values_to_send, unavail);
			values_to_send.remove_all();
			idle_scheduled = false;
			return false;
		}

		public void Subscribe (string[] keys, out HashTable<string, Value?> values, out string[] undeterminable_keys) {
			StringSet keyset = manager.check_keys(keys);
			debug("Subscribe: requested valid keys %s", keyset.debug());
			StringSet new_keys = new StringSet.difference (keyset, subscribed_keys);
			debug("Subscribe: new keys %s", new_keys.debug());
			key_counter.add (new_keys);

			subscribed_keys = new StringSet.union(subscribed_keys, new_keys);

			// Read the values from the central value table and return them
			// Note: Use also those keys which were already subscribed to (and are subscribed to again)
			manager.get_internal (keyset, out values, out undeterminable_keys);
		}

		public void Unsubscribe (string[] keys) {
			StringSet keyset = manager.check_keys(keys);
			StringSet decreasing_keys = new StringSet.intersection (keyset, subscribed_keys);
			key_counter.remove (decreasing_keys);
			subscribed_keys = new StringSet.difference (subscribed_keys, decreasing_keys);
		}

		/* Is called when the value of a property changes */
		public void on_value_changed(string key, Value? value) {
			if (subscribed_keys.is_member (key)) {
				values_to_send.insert (key, value);
				if (!idle_scheduled) {
					Idle.add(this.emit_changed);
					idle_scheduled = true;
				}
			}
		}
	}
}
