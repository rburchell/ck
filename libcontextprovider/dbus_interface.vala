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
using DBus;

namespace ContextProvider {

	[DBus (name = "org.freedesktop.ContextKit.Manager")]
	public interface DBusManager : GLib.Object {
		public abstract DBus.ObjectPath GetSubscriber (DBus.BusName name) throws DBus.Error;
	}

	[DBus (name = "org.freedesktop.ContextKit.Subscriber")]
	public interface DBusSubscriber: GLib.Object {
		public abstract void Subscribe (string[] keys, out HashTable<string, Value?> values, out string[] undeterminable_keys);

		public abstract void Unsubscribe (string[] keys);
		public signal void Changed (HashTable<string, Value?> values, string[] undeterminable_keys);
	}
}
