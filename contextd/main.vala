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

namespace ContextD {

	public class Main : GLib.Object {

		private static Gee.ArrayList<Plugin> plugins;
		private static GLib.MainLoop loop;	

		private void install_plugins () {

			// Command all plugins to install themselves.
			// Each plugin knows which keys it provides and
			// what parameters need to be passed to ContextProvider.install.
			foreach (var plugin in plugins) {
				plugin.install ();
			}
		}

		static void exit (int sig) {
			stdout.printf("contextd exiting");
			loop.quit();
		}

		private void run () {
			loop = new MainLoop (null, false);
			stdout.printf ("contextd starting");
			
			// Start using the licontextprovider
			ContextProvider.init(DBus.BusType.SESSION, "org.freedesktop.ContextKit");

			// Install all plugins
			install_plugins ();
			
			//Posix.Signal.set_handler (Posix.Signal.QUIT, exit);
			loop.run ();
		}

		static int main (string[] args) {
			
			// Define the set of plugins
			plugins = new Gee.ArrayList<Plugin> ();
			plugins.add(new Plugins.HalPlugin ());

			// TODO: define the used plugins based on which
			// resources are detected during configure phase.

			var main = new Main ();
			main.run ();
			return 0;
		}
	}
}