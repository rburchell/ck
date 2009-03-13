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
 * Author:
 *	Rob Taylor <rob.taylor@codethink.co.uk>
 */

using GLib;
using Posix.Signal;
using ContextProvider;

namespace ContextKit {

		public class Main : Object {

		static GLib.MainLoop loop;
		static dynamic DBus.Object bus; // Needs to be stored so that we get the NameOwnerChanged

		public Main () {
		}

		private static bool start_manager () {
			try {
				var connection = DBus.Bus.get (DBus.BusType.SESSION);
				bus = connection.get_object ( "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
				// try to register service in session bus
				uint request_name_result = bus.RequestName ("org.freedesktop.ContextKit", (uint) 0);

				if (request_name_result == DBus.RequestNameReply.PRIMARY_OWNER) {
					debug ("Creating new Manager D-Bus service");

					Manager? manager = Manager.get_instance ();
					manager.providers.register (new MCE.Provider());

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

		static void exit (int sig){
			stdout.printf("Exit contextd");
			loop.quit();
		}

		public void run () {
			loop = new MainLoop (null, false);
			stdout.printf ("Hello, world!\n");
			start_manager();
			Posix.Signal.set_handler(Posix.Signal.QUIT,exit);
			loop.run();
		}

		static int main (string[] args) {
			var main = new Main ();
			main.run ();
			return 0;
		}

	}
}
