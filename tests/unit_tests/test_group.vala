/* test_group.vala
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
using ContextProvider;

void test_group_keys() {
	string[] keys = {"a","b","c"};
	Group g = new Group(keys, false, null);
	StringSet keyset = new StringSet.from_array(keys);
	assert (keyset.is_equal(g.keys));
}


void test_group_null_subscribe() {
	string[] keys = {"a","b","c"};
	Group g = new Group(keys, false, null);
	g.subscribe(true);
	g.subscribe(false);
}

class Tester {
	public int subscribe_count = 0;
	public bool clear_signal_emitted = false;
	public StringSet? cleared_keys = null;

	public void callback (bool subscribe) {
		if (subscribe) {
			subscribe_count++;
		} else {
			subscribe_count--;
		}
	}
	public void clear_signal (Group g, StringSet keys) {
		cleared_keys = keys;
		clear_signal_emitted = true;
	}
}

void test_group_subscribe() {
	string[] keys = {"a","b","c"};
	Tester t = new Tester();
	Group g = new Group(keys, false, t.callback);
	g.subscribe(true);
	assert (t.subscribe_count == 1);
	g.subscribe(true);
	assert (t.subscribe_count == 1);
	g.subscribe(false);
	assert (t.subscribe_count == 0);
	g.subscribe(false);
	assert (t.subscribe_count == 0);
}

void test_group_clear_signal() {
	string[] keys = {"a","b","c"};
	Tester t = new Tester();
	Group g = new Group(keys, true, t.callback);
	StringSet check_keys  = new StringSet.from_array(keys);

	g.clear_values += t.clear_signal;

	g.subscribe(true);
	assert (t.clear_signal_emitted);
	assert (t.cleared_keys.is_equal(check_keys));
	t.clear_signal_emitted = false;
	g.subscribe(true);
	assert (!t.clear_signal_emitted);
	g.subscribe(false);
	assert (!t.clear_signal_emitted);
	g.subscribe(false);
	assert (!t.clear_signal_emitted);
}


public static void main (string[] args) {
	Test.init (ref args);
	Test.add_func("/contextkit/group/keys", test_group_keys);
	Test.add_func("/contextkit/group/null_subscribe", test_group_null_subscribe);
	Test.add_func("/contextkit/group/subscribe", test_group_subscribe);
	Test.add_func("/contextkit/group/clear_signal", test_group_clear_signal);
	Test.run ();
}

