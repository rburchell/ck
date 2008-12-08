/* main.vala
 *
 * Copyright (C) 2008  Rob Taylor
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:
 *	Rob Taylor <rob.taylor@codethink.co.uk>
 */

using GLib;
using Sqlite;

namespace ContextKit {

	public class Main : Object {
		public Main () {
		}

		private static ContextKit.Manager manager;

		private static bool start_manager () {
			try {
				var connection = DBus.Bus.get (DBus.BusType.SESSION);
				dynamic DBus.Object bus = connection.get_object ( "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
				// try to register service in session bus
				uint request_name_result = bus.RequestName ("org.freedesktop.ContextKit", (uint) 0);

				if (request_name_result == DBus.RequestNameReply.PRIMARY_OWNER) {
					debug ("Creating new Manager D-Bus service");
					manager = new ContextKit.Manager();
					connection.register_object ("/org/freedesktop/ContextKit/Manager", manager);
				} else {
					debug ("Manager D-Bus service is already running");
					return false;
				}

			} catch (Error e) {
				error ("Oops %s", e.message); return false;
			}

			return true;
		}

		public void run () {
			var loop = new MainLoop (null, false);
			stdout.printf ("Hello, world!\n");
			start_manager();
			loop.run();
		}

		static int main (string[] args) {
			var main = new Main ();
			main.run ();
			return 0;
		}

	}
}
