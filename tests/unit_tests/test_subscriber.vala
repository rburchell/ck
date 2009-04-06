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

using ContextProvider;

int changed_count = 0;
HashTable<string, Value?> last_changed_values;
string[] last_changed_undeterminable;
GLib.MainLoop loop;

void changed_handler(Subscriber s, HashTable<string, Value?> values, string[] undeterminable_keys)
{
	debug("changed");
// Record that the signal was caught
	++changed_count;

	// Store the parameters
	last_changed_values = new HashTable<string, Value?>(str_hash, str_equal);
	foreach (var key in values.get_keys()) {
		last_changed_values.insert(key, values.lookup(key));
	}

	last_changed_undeterminable = undeterminable_keys;
}

bool stopper()
{
	loop.quit ();
	return false;
}

void test_subscribe()
{
	// Clear results
	changed_count = 0;

	// Setup
	Manager manager = new Manager();
	Group group = new Group({"key.one", "key.two", "key.three"}, false, null);
	manager.group_list.add(group);
	KeyUsageCounter kc = new KeyUsageCounter();

	Subscriber s = new Subscriber(manager, kc, 7); 
	s.Changed += changed_handler;

	// Test:
	HashTable<string, Value?> ret_values;
	string[] ret_undeterminable;

	// subscribe to a key
	s.Subscribe({"key.three"}, out ret_values, out ret_undeterminable);
	
	// Expected result: the subscription count of the key
	// increases. 
	assert(changed_count == 0);
	assert(kc.number_of_subscribers("key.one") == 0);
	assert(kc.number_of_subscribers("key.two") == 0);
	assert(kc.number_of_subscribers("key.three") == 1);
}

void test_subscribe_twice()
{
	// Clear results
	changed_count = 0;

	// Setup
	Manager manager = new Manager();
	Group group = new Group({"key.one", "key.two", "key.three"}, false, null);
	manager.group_list.add(group);
	KeyUsageCounter kc = new KeyUsageCounter();

	Subscriber s = new Subscriber(manager, kc, 7); 
	s.Changed += changed_handler;
	
	// Test:
	HashTable<string, Value?> ret_values;
	string[] ret_undeterminable;

	// subscribe to a key
	s.Subscribe({"key.three"}, out ret_values, out ret_undeterminable);
	s.Subscribe({"key.three"}, out ret_values, out ret_undeterminable);
	
	// Expected result: the subscription count of the key
	// is only 1, not 2.
	assert(changed_count == 0);
	assert(kc.number_of_subscribers("key.one") == 0);
	assert(kc.number_of_subscribers("key.two") == 0);
	assert(kc.number_of_subscribers("key.three") == 1);
}

void test_subscribe_invalid()
{
	// Clear results
	changed_count = 0;

	// Setup
	Manager manager = new Manager();
	Group group = new Group({"key.one", "key.two", "key.three"}, false, null);
	manager.group_list.add(group);
	KeyUsageCounter kc = new KeyUsageCounter();

	Subscriber s = new Subscriber(manager, kc, 7); 
	s.Changed += changed_handler;
	
	// Test:
	HashTable<string, Value?> ret_values;
	string[] ret_undeterminable;

	// subscribe to an invalid key
	s.Subscribe({"key.invalid"}, out ret_values, out ret_undeterminable);
		
	// Expected result: the subscription count of the invalid key
	// is still 0.
	assert(changed_count == 0);
	assert(kc.number_of_subscribers("key.one") == 0);
	assert(kc.number_of_subscribers("key.two") == 0);
	assert(kc.number_of_subscribers("key.three") == 0);
	assert(kc.number_of_subscribers("key.invalid") == 0);
}

void test_subscribe_return_values()
{
	// Clear results
	changed_count = 0;

	// Setup
	Manager manager = new Manager();
	Group group = new Group({"key.one", "key.two", "key.three"}, false, null);
	manager.group_list.add(group);
	KeyUsageCounter kc = new KeyUsageCounter();
	// Set the value for the key "key.one"
	Value keyOneValue = Value(typeof(int));
	keyOneValue.set_int(111);
	manager.property_value_change("key.one", keyOneValue); 

	Subscriber s = new Subscriber(manager, kc, 7); 
	s.Changed += changed_handler;
	
	// Test:
	HashTable<string, Value?> ret_values;
	string[] ret_undeterminable;

    // subscribe to 3 keys: one has a value, one is undeterminable, and one is invalid
	s.Subscribe({"key.one", "key.two", "key.invalid"}, out ret_values, out ret_undeterminable);
	
	// Expected result: the return values of Subscribe:
	// The value for key.one
	// key.two as undetermined
	// key.invalid is not present

	assert (ret_values.size() == 1);
	assert (value_compare(ret_values.lookup ("key.one"), keyOneValue) == true);
	assert (ret_undeterminable.length == 1);
	assert (ret_undeterminable[0] == "key.two");
}

void test_changing_value()
{
	// Clear results
	changed_count = 0;

	// Setup
	Manager manager = new Manager();
	Group group = new Group({"key.one", "key.two", "key.three"}, false, null);
	manager.group_list.add(group);
	KeyUsageCounter kc = new KeyUsageCounter();
	// Set the value for the key "key.one"
	Value keyOneValue = Value(typeof(int));
	keyOneValue.set_int(111);
	manager.property_value_change("key.one", keyOneValue); 

	Subscriber s = new Subscriber(manager, kc, 7); 
	s.Changed += changed_handler;
	
	// Test:
	HashTable<string, Value?> ret_values;
	string[] ret_undeterminable;

    // subscribe to 3 keys: one has a value, one is undeterminable, and one is invalid
	s.Subscribe({"key.one", "key.two", "key.invalid"}, out ret_values, out ret_undeterminable);
	
	// make the value of key.one to change
	Value keyOneNewValue = Value(typeof(int));
	keyOneNewValue.set_int (-9);
	s.on_value_changed ("key.one", keyOneNewValue);

	// ... and the value for key.three as well (this key is not subscribed to)
	Value keyThreeNewValue = Value(typeof(int));
	keyThreeNewValue.set_int (88);
	s.on_value_changed ("key.three", keyThreeNewValue);

	Idle.add(stopper);
	loop = new GLib.MainLoop(null, false);
	loop.run();
	
	// Expected result: we receive the changed signal with the values.
	// The value for key.one
	// no entries for other keys
	assert (changed_count == 1);
	assert (last_changed_values.size() == 1);
	assert (value_compare(last_changed_values.lookup ("key.one"), keyOneNewValue) == true);
	assert (last_changed_undeterminable.length == 0);
}

void test_changing_to_undeterminable()
{
	// Clear results
	changed_count = 0;

	// Setup
	Manager manager = new Manager();
	Group group = new Group({"key.one", "key.two", "key.three"}, false, null);
	manager.group_list.add(group);
	KeyUsageCounter kc = new KeyUsageCounter();
	// Set the value for the key "key.one"
	Value keyOneValue = Value(typeof(int));
	keyOneValue.set_int(111);
	manager.property_value_change("key.one", keyOneValue); 

	Subscriber s = new Subscriber(manager, kc, 7); 
	s.Changed += changed_handler;
	
	// Test:
	HashTable<string, Value?> ret_values;
	string[] ret_undeterminable;

    // subscribe to 3 keys: one has a value, one is undeterminable, and one is invalid
	s.Subscribe({"key.one", "key.two", "key.invalid"}, out ret_values, out ret_undeterminable);
	
	// make the value of key.one to change to undeterminable
	s.on_value_changed ("key.one", null);

	Idle.add(stopper);
	loop = new GLib.MainLoop(null, false);
	loop.run();
	
	// Expected result: we receive the changed signal with the values.
	// key.one as undeterminable
	// no entries for other keys
	assert (changed_count == 1);
	assert (last_changed_values.size() == 0);
	assert (last_changed_undeterminable.length == 1);
	assert (last_changed_undeterminable[0] == "key.one");
}

void test_changing_to_determinable()
{
	// Clear results
	changed_count = 0;

	// Setup
	Manager manager = new Manager();
	Group group = new Group({"key.one", "key.two", "key.three"}, false, null);
	manager.group_list.add(group);
	KeyUsageCounter kc = new KeyUsageCounter();
	// Set the value for the key "key.one"
	Value keyOneValue = Value(typeof(int));
	keyOneValue.set_int(111);
	manager.property_value_change("key.one", keyOneValue); 

	Subscriber s = new Subscriber(manager, kc, 7); 
	s.Changed += changed_handler;
	
	// Test:
	HashTable<string, Value?> ret_values;
	string[] ret_undeterminable;

    // subscribe to 3 keys: one has a value, one is undeterminable, and one is invalid
	s.Subscribe({"key.one", "key.two", "key.invalid"}, out ret_values, out ret_undeterminable);
	
	// make the value of key.two to change from undeterminable to a value
	Value keyTwoValue = Value(typeof(double));
	keyTwoValue.set_double(-0.2);
	s.on_value_changed ("key.two", keyTwoValue);

	Idle.add(stopper);
	loop = new GLib.MainLoop(null, false);
	loop.run();
	
	// Expected result: we receive the changed signal with the values.
	// The value for key.two
	// no entries for other keys
	assert (changed_count == 1);
	assert (last_changed_values.size() == 1);
	assert (value_compare(last_changed_values.lookup ("key.two"), keyTwoValue) == true);
	assert (last_changed_undeterminable.length == 0);
}

void test_changing_two_values()
{
	// Clear results
	changed_count = 0;

	// Setup
	Manager manager = new Manager();
	Group group = new Group({"key.one", "key.two", "key.three"}, false, null);
	manager.group_list.add(group);
	KeyUsageCounter kc = new KeyUsageCounter();
	// Set the value for the key "key.one"
	Value keyOneValue = Value(typeof(int));
	keyOneValue.set_int(111);
	manager.property_value_change("key.one", keyOneValue); 
	// and the value for the key "key.two"
	Value keyTwoValue = Value(typeof(double));
	keyTwoValue.set_double(-3.2);
	manager.property_value_change("key.two", keyTwoValue); 

	Subscriber s = new Subscriber(manager, kc, 7); 
	s.Changed += changed_handler;
	
	// Test:
	HashTable<string, Value?> ret_values;
	string[] ret_undeterminable;

    // subscribe to 3 keys: one has a value, one is undeterminable, and one is invalid
	s.Subscribe({"key.one", "key.two", "key.invalid"}, out ret_values, out ret_undeterminable);
	
	// make the values of key.one and key.two to change
	Value keyOneNewValue = Value(typeof(int));
	keyOneNewValue.set_int(8);
	s.on_value_changed ("key.one", keyOneNewValue);
	Value keyTwoNewValue = Value(typeof(double));
	keyTwoNewValue.set_double(6.3);
	s.on_value_changed ("key.two", keyTwoNewValue);

	Idle.add(stopper);
	loop = new GLib.MainLoop(null, false);
	loop.run();
	
	// Expected result: we receive the changed signal with the values.
	// The values for key.one and key.two
	// no entries for other keys
	assert (changed_count == 1);
	assert (last_changed_values.size() == 2);
	assert (value_compare(last_changed_values.lookup ("key.one"), keyOneNewValue) == true);
	assert (value_compare(last_changed_values.lookup ("key.two"), keyTwoNewValue) == true);
	assert (last_changed_undeterminable.length == 0);
}

public static void main (string[] args) {
       Test.init (ref args);
       Test.add_func ("/contextkit/subscriber/test_subscribe", test_subscribe);
       Test.add_func ("/contextkit/subscriber/test_subscribe_twice", test_subscribe_twice);
	   Test.add_func ("/contextkit/subscriber/test_subscribe_invalid", test_subscribe_invalid);
	   Test.add_func ("/contextkit/subscriber/test_subscribe_return_values", test_subscribe_return_values);
	   Test.add_func ("/contextkit/subscriber/test_changing_value", test_changing_value);
	   Test.add_func ("/contextkit/subscriber/test_changing_to_undeterminable", test_changing_to_undeterminable);
	   Test.add_func ("/contextkit/subscriber/test_changing_to_determinable", test_changing_to_determinable);
	   Test.add_func ("/contextkit/subscriber/test_changing_two_values", test_changing_two_values);
       Test.run ();
}
