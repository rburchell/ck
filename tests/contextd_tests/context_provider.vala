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

	public string log;

	// Mocked functions
	public void set_integer (string key, int value) {
		string newLog = "set_integer(%s,%d)".printf(key, value);
		log += newLog;
	}

	public void set_double (string key, double value) {
		string newLog = "set_double(%s,%f)".printf(key, value);
		log += newLog;
	}

	public void set_boolean (string key, bool value) {
	}

	public void set_string (string key, string value) {
	}

	public void set_null (string key) {
	}

	public delegate void SubscriptionChangedCallback(bool subscribe);

	public void install_group ([CCode (array_length = false, array_null_terminated = true)] string[] key_group, bool clear_values_on_subscribe, SubscriptionChangedCallback? subscription_changed_cb) {
	}

	public void install_key(string key, bool clear_values_on_subscribe, SubscriptionChangedCallback? subscription_changed_cb) {
	}
}
