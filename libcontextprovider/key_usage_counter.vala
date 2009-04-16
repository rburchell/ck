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

	/**
	 * SECTION:KeyUsageCounter
	 * @short_description: A subscription counter for keys.
	 * 
	 * A data structure which tracks the number of subscriptions for 
	 * keys.
	 */

	internal class KeyUsageCounter : GLib.Object {
	/**
	 * ContextProviderKeyUsageCounter
	 *
	 * A data structure which tracks the number of subscriptions for
	 * keys.
	 */

		// The number of subscribers for each key (over all subscriber objects)
		internal Gee.HashMap<string, int> subscriber_count_table {get; private set;}
		public StringSet subscribed_keys { get; private set; }

		public signal void keys_added (StringSet new_keys);
		public signal void keys_removed (StringSet keys_removed, StringSet keys_remaining);

		/**
		 * Constructs a new KeyUsageCounter.
		 */
		public KeyUsageCounter() {
			subscribed_keys = new StringSet();
			subscriber_count_table = new Gee.HashMap<string, int>(str_hash, str_equal);
		}

		/**
		 * @key: a string
		 * Increases the subscription count of the #key.
		 */
		public void add (StringSet keys) {
			StringSet first_subscribed_keys = new StringSet();

			foreach (var key in keys) {
				if (subscriber_count_table.contains (key)) {
					int old_value = subscriber_count_table.get (key);
					subscriber_count_table.set (key, old_value + 1);

					if (old_value == 0) {
						// This is the first subscribed to the key
						first_subscribed_keys.add (key);
					}

					debug ("Subscriber count of %s is now %d", key, old_value + 1);
				}
				else { // Key name not present in the key table
					subscriber_count_table.set (key, 1);

					first_subscribed_keys.add (key);
					debug ("Subscriber count of %s is now %d", key, 1);
				}
			}

			if (first_subscribed_keys.size() > 0) {
				keys_added (first_subscribed_keys);
			}

			subscribed_keys = new StringSet.union (subscribed_keys, first_subscribed_keys);
		}

		/**
		 * @key: a string
		 * Decreases the subscription count of the #key.
		 */
		public void remove (StringSet keys) {
			StringSet last_unsubscribed_keys = new StringSet();

			foreach (var key in keys) {
				if (subscriber_count_table.contains (key)) {
					int value = subscriber_count_table.get (key);

					if (value >= 1) {
						value--;
						subscriber_count_table.set (key, value);
					}

					if (value == 0) {
						// The last subscriber for this key unsubscribed
						last_unsubscribed_keys.add (key);
					}
					debug ("Subscriber count of %s is now %d", key, value);
				}
				else {
					// This should not happen
					debug ("Error: decreasing the subscriber count of an unknown key.");
				}
			}

			subscribed_keys = new StringSet.difference (subscribed_keys, last_unsubscribed_keys);

			if (last_unsubscribed_keys.size() > 0) {
				// Signal that some keys are not subscribed to anymore
				keys_removed (last_unsubscribed_keys, subscribed_keys);
			}

		}

		/**
		 * @key: a string
		 * Returns: The subscription count of the #key.
		 */
		public int number_of_subscribers (string key) {
			if (subscriber_count_table.contains (key)) {
				return subscriber_count_table.get (key);
			}
			return 0;
		}
	}
}
