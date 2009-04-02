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

	public class GroupList {
		// List of groups
		Gee.ArrayList<Group> groups = new Gee.ArrayList<Group> ();

		// Valid keys provided by all groups
		public StringSet valid_keys {get; private set;}

		public GroupList () {
			valid_keys = new StringSet();
		}

		public void add (Group group) {
			//debug ("New group registered: %s (%p)", group.keys.debug(), group);
			groups.add (group);
			valid_keys = new StringSet.union (valid_keys, group.keys);
		}

		public void remove (Group group) {
			//debug ("Group unregistered: %s (%p)", group.keys.debug(), group);
			groups.remove (group);
			StringSet s = new StringSet();

			/* a bit inefficient, but it'll do for now...
			   TODO: make the stringset stuff not use ctors..
			 */
			foreach (var g in groups) {
				s = new StringSet.union (s, g.keys);
			}
			valid_keys = s;
		}

		public void first_subscribed(StringSet keys) {
			foreach (var group in groups) {
				StringSet intersection = new StringSet.intersection (keys, group.keys);
				if (intersection.size() > 0 && !group.subscribed) {
					group.subscribe(true);
				}
			}
		}

		public void last_unsubscribed(StringSet keys_unsubscribed, StringSet keys_remaining) {
			foreach (var group in groups) {
				StringSet intersection = new StringSet.intersection (keys_unsubscribed, group.keys);
				if (intersection.size() > 0 ) {
					group.subscribe (false);
				}
			}
		}
	}
}
