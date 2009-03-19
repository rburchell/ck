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

	public class Providers {
		// List of providers
		Gee.ArrayList<Provider> providers = new Gee.ArrayList<Provider> ();

		// Valid keys provided by all providers
		public StringSet valid_keys {get; private set;}

		internal Providers () {
			valid_keys = new StringSet();
		}

		public void register (Provider provider) {
			debug ("New provider registered: %s (%p)", provider.provided_keys().debug(), provider);
			providers.add (provider);
			valid_keys = new StringSet.union (valid_keys, provider.provided_keys());
		}

		public void unregister (Provider provider) {
			debug ("Provider unregistered: %s (%p)", provider.provided_keys().debug(), provider);
			providers.remove (provider);
			StringSet s = new StringSet();

			/* a bit inefficient, but it'll do for now...
			   TODO: make the stringset stuff not use ctors..
			 */
			foreach (var p in providers) {
				s = new StringSet.union (s, p.provided_keys());
			}
			valid_keys = s;
		}

		public ArrayList<string> get (StringSet keys, HashTable<string, Value?> values) {
			debug("Providers.get called (%s)", keys.debug());
			ArrayList<string> unavail = new ArrayList<string> ();
			foreach (var provider in providers) {
				StringSet intersection = new StringSet.intersection (keys, provider.provided_keys());
				if (intersection.size() > 0) {
					debug ("calling provider %p", provider);
					provider.get (keys, values, unavail);
				}
			}
			return unavail;
		}

		public void first_subscribed(StringSet keys) {
			foreach (var provider in providers) {
				StringSet intersection = new StringSet.intersection (keys, provider.provided_keys());
				if (intersection.size() > 0 ) {
					provider.keys_subscribed (keys);
				}
			}
		}

		public void last_unsubscribed(StringSet keys_unsubscribed, StringSet keys_remaining) {
			foreach (var provider in providers) {
				StringSet intersection = new StringSet.intersection (keys_unsubscribed, provider.provided_keys());
				if (intersection.size() > 0 ) {
					StringSet intersection_remaining = new StringSet.intersection (keys_remaining, provider.provided_keys());
					provider.keys_unsubscribed (intersection, intersection_remaining);
				}
			}
		}
	}
}
