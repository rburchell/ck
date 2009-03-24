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

namespace ContextProvider {

	public class KeyUsageCounter {
		// The number of subscribers for each key (over all subscriber objects)
		Gee.HashMap<string, int> no_of_subscribers = new Gee.HashMap<string, int>(str_hash, str_equal);
		public StringSet subscribed_keys { get; private set; }

		public KeyUsageCounter() {
			subscribed_keys = new StringSet();
		}

		public void add (StringSet keys) {
			StringSet first_subscribed_keys = new StringSet();

			foreach (var key in keys) {
				if (no_of_subscribers.contains (key)) {
					int old_value = no_of_subscribers.get (key);
					no_of_subscribers.set (key, old_value + 1);

					if (old_value == 0) {
						// This is the first subscribed to the key
						first_subscribed_keys.add (key);
					}

					debug ("Subscriber count of %s is now %d", key, old_value + 1);
				}
				else { // Key name not present in the key table
					no_of_subscribers.set (key, 1);

					first_subscribed_keys.add (key);
					debug ("Subscriber count of %s is now %d", key, 1);
				}
			}

			if (first_subscribed_keys.size() > 0) {
				// Signal that some new keys were subscribed to
				Manager.get_instance().providers.first_subscribed (first_subscribed_keys);
			}

			subscribed_keys = new StringSet.union (subscribed_keys, first_subscribed_keys);
		}

		public void remove (StringSet keys) {
			StringSet last_unsubscribed_keys = new StringSet();

			foreach (var key in keys) {
				if (no_of_subscribers.contains (key)) {
					int old_value = no_of_subscribers.get (key);

					if (old_value >= 1) {
						// Do not store negative values
						no_of_subscribers.set (key, old_value - 1);
					}

					if (old_value <= 1) {
						// The last subscriber for this key unsubscribed
						last_unsubscribed_keys.add (key);
					}
					debug ("Subscriber count of %s is now %d", key, old_value - 1);
				}
				else {
					// This should not happen
					debug ("Error: decreasing the subscriber count of an unknown key.");
				}
			}

			subscribed_keys = new StringSet.difference (subscribed_keys, last_unsubscribed_keys);

			if (last_unsubscribed_keys.size() > 0) {
				// Signal that some keys are not subscribed to anymore
				Manager.get_instance().providers.last_unsubscribed (last_unsubscribed_keys, subscribed_keys);
			}

		}

		public int number_of_subscribers (string key) {
			if (no_of_subscribers.contains (key)) {
				return no_of_subscribers.get (key);
			}
			return 0;
		}
	}
}
