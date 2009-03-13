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
	/**
	 * ContextProviderProvider:
	 *
	 * Represents a provider of context
	 */
	public interface Provider : GLib.Object {
		public abstract void get (StringSet keys, HashTable<string, Value?> ret, ArrayList<string> unavailable_keys);
		public abstract void keys_subscribed (StringSet keys_subscribed);
		public abstract void keys_unsubscribed (StringSet keys_unsubscribed, StringSet keys_left);
		public abstract StringSet provided_keys();
	}
}
