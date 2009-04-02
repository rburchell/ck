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

int added_count = 0;
int removed_count = 0;

StringSet last_added_parameter;
StringSet last_removed_parameter1;
StringSet last_removed_parameter2;

void keys_added_slot(KeyUsageCounter k, StringSet new_keys) {
	++added_count;
	last_added_parameter = new_keys;
}

void keys_removed_slot(KeyUsageCounter k, StringSet removed_keys, StringSet remaining_keys) {
	++removed_count;
	last_removed_parameter1 = removed_keys;
	last_removed_parameter2 = remaining_keys;
}
	

void test_add() {
	// Clear results
	added_count = 0;
	removed_count = 0;

	// Setup
	KeyUsageCounter kc = new KeyUsageCounter(); 
	kc.keys_added += keys_added_slot;
	kc.keys_removed += keys_removed_slot;

	// Test:
	// increase the count for one key
	kc.add(new StringSet.from_array({"key.one"}));
	
	// Expected result: the keys_added is emitted once 
	// to notify that this key was subscribed to.
	assert(added_count == 1);
	assert(last_added_parameter.size() == 1);
	assert(last_added_parameter.is_equal(new StringSet.from_array({"key.one"})));
	assert(removed_count == 0);
}

void test_add_and_remove() {
	// Clear results
	added_count = 0;
	removed_count = 0;

	// Setup
	KeyUsageCounter kc = new KeyUsageCounter(); 
	kc.keys_added += keys_added_slot;
	kc.keys_removed += keys_removed_slot;

	// Test: 
	// increase the count for one key
	kc.add(new StringSet.from_array({"key.two"}));
	// decrease the count for the same key
	kc.remove(new StringSet.from_array({"key.two"}));

	// Expected result: both signals are emitted once
	// to notify that this key was subscribed to
	// and then unsubscribed from
	assert(added_count == 1);
	assert(last_added_parameter.is_equal(new StringSet.from_array({"key.two"})));
	assert(removed_count == 1);
	assert(last_removed_parameter1.is_equal(new StringSet.from_array({"key.two"})));
	assert(last_removed_parameter2.is_equal(new StringSet.from_array({})));
}

void test_remaining() {
	// Clear results
	added_count = 0;
	removed_count = 0;

	// Setup
	KeyUsageCounter kc = new KeyUsageCounter(); 
	kc.keys_added += keys_added_slot;
	kc.keys_removed += keys_removed_slot;

	// Test: 
	// increase the count for two keys
	kc.add (new StringSet.from_array({"key.two", "key.three"}));
	// decrease the count for the one of them
	kc.remove (new StringSet.from_array({"key.two"}));

	// Expected result: both signals are emitted once
	// to notify that two keys were subscribed to
	// and then one of them was unsubscribed from
	assert (added_count == 1);
	assert (last_added_parameter.is_equal(new StringSet.from_array({"key.two", "key.three"})));
	assert (removed_count == 1);
	assert (last_removed_parameter1.is_equal(new StringSet.from_array({"key.two"})));
	assert (last_removed_parameter2.is_equal(new StringSet.from_array({"key.three"})));
}

public static void main (string[] args) {
       Test.init (ref args);
       Test.add_func("/contextkit/key_usage_counter/test_add", test_add);
       Test.add_func("/contextkit/key_usage_counter/test_add_and_remove", test_add_and_remove);
	   Test.add_func("/contextkit/key_usage_counter/test_remaining", test_remaining);
       Test.run ();
}
