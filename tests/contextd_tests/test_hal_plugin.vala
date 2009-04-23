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

void test_install() {
	// Setup
	
	// Test
	Plugins.HalPlugin plugin = new Plugins.HalPlugin();
	bool success = plugin.install ();

	// Expected results: the installation succeeded
	assert (success == true);
}

void debug_null  (string? log_domain, LogLevelFlags log_level, string message)
{
}

public static void main (string[] args) {
	Test.init (ref args);

	if (Test.quiet()) {
		Log.set_handler ("ContextKit", LogLevelFlags.LEVEL_DEBUG | LogLevelFlags.FLAG_RECURSION, debug_null);
	}

	Test.add_func("/contextd/hal_plugin/test_install", test_install);
	Test.run ();
}
