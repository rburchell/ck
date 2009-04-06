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
using ContextProvider;

void test_key_checking()
{
	// Setup
	Manager manager = new Manager();
	Group group = new Group({"key.one", "key.two", "key.three"}, false, null);
	manager.group_list.add(group);

	// Test: check the validity of two valid and one invalid keys
	StringSet valid_keys = manager.check_keys({"key.three", "key.one", "key.invalid"});

	// Expected result: the checked keyset contains the two valid keys

	assert (valid_keys.size() == 2);
	assert (valid_keys.is_member("key.one") == true);
	assert (valid_keys.is_member("key.three") == true);
}

void test_get_values()
{
	// Setup
	Manager manager = new Manager();
	Group group = new Group({"key.one", "key.two", "key.three", "key.four", "key.five"}, false, null);
	manager.group_list.add(group);

	// Test: set the values for the keys
	Value keyOneValue = Value(typeof(int));
	keyOneValue.set_int(123);
	manager.property_value_change("key.one", keyOneValue);

	Value keyTwoValue = Value(typeof(double));
	keyTwoValue.set_double(-3.2);
	manager.property_value_change("key.two", keyTwoValue);

	Value keyThreeValue = Value(typeof(bool));
	keyThreeValue.set_boolean(false);
	manager.property_value_change("key.three", keyThreeValue);

	Value keyFourValue = Value(typeof(string));
	keyFourValue.set_string("my value");
	manager.property_value_change("key.four", keyFourValue);

	manager.property_value_change("key.five", null);

	// and query the values for the keys
	HashTable<string, Value?> properties;
	string[] undeterminable;
	manager.get_internal(new StringSet.from_array({"key.one", "key.two", "key.three", "key.four", "key.five"}), out properties, out undeterminable);

	// Expected result: the values are retrieved properly
	assert (properties.size() == 4);
	assert (value_compare(properties.lookup("key.one"), keyOneValue) == true);
	assert (value_compare(properties.lookup("key.two"), keyTwoValue) == true);
	assert (value_compare(properties.lookup("key.three"), keyThreeValue) == true);
	assert (value_compare(properties.lookup("key.four"), keyFourValue) == true);
	assert (undeterminable.length == 1);
	assert (undeterminable[0] == "key.five");
}

void test_get_subscriber()
{
	// Setup
	Manager manager = new Manager();

//	try {
		// Test: get subscriber twice, for different bus names
		DBus.BusName name1 = new DBus.BusName(":1.2");
		DBus.ObjectPath path1 = manager.GetSubscriber(name1);

		DBus.BusName name2 = new DBus.BusName(":1.12");
		DBus.ObjectPath path2 = manager.GetSubscriber(name2);

		// Expected result: the object paths are paths for two first subscribers
		assert (path1 == "/org/freedesktop/ContextKit/Subscribers/0");
		assert (path2 == "/org/freedesktop/ContextKit/Subscribers/1");
/*	} catch (DBus.Error e) {
		critical("%s", e.message);
		assert(false);
	}*/
}

void test_get_subscriber_twice()
{
	// Setup
	Manager manager = new Manager();

	//try {
		// Test: get subscriber twice, for the same bus name
		DBus.BusName name1 = new DBus.BusName(":1.2");
		DBus.ObjectPath path1 = manager.GetSubscriber(name1);
		DBus.ObjectPath path2 = manager.GetSubscriber(name1);

		// Expected result: the object paths are the same
		assert (path1 == "/org/freedesktop/ContextKit/Subscribers/0");
		assert (path2 == "/org/freedesktop/ContextKit/Subscribers/0");
	/*} catch (DBus.Error e) {
		critical("%s", e.message);
		assert(false);
	}*/
}

void debug_null  (string? log_domain, LogLevelFlags log_level, string message)
{
}

public static void main (string[] args) {
	Test.init (ref args);

	if (Test.quiet()) {
		Log.set_handler ("ContextKit", LogLevelFlags.LEVEL_DEBUG | LogLevelFlags.FLAG_RECURSION, debug_null);
	}

	Test.add_func ("/contextkit/manager/test_key_checking", test_key_checking);
	Test.add_func ("/contextkit/manager/test_get_values", test_get_values);
	Test.add_func ("/contextkit/manager/test_get_subscriber", test_get_subscriber);
	Test.add_func ("/contextkit/manager/test_get_subscriber_twice", test_get_subscriber_twice);
	Test.run ();
}
