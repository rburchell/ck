/* test_grouplist.vala
*
* Copyright (C) 2008, 2009 Nokia Corporation.
*
* Contact: Marius Vollmer <marius.vollmer@nokia.com>
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

void test_grouplist_add() {
	GroupList gl = new GroupList();

	string[] keys = {"a","b","c"};
	Group g = new Group(keys, false, null);
	gl.add(g);

	assert (g.keys.is_subset_of(gl.valid_keys));

	string[] keys2 = {"d","f","c"};
	Group g2 = new Group(keys2, false, null);
	gl.add(g2);

	assert (g2.keys.is_subset_of(gl.valid_keys));
}

void test_grouplist_remove() {
	GroupList gl = new GroupList();

	string[] keys = {"a","b","c"};
	Group g1 = new Group(keys, false, null);
	gl.add(g1);

	string[] keys2 = {"d","f","c"};
	Group g2 = new Group(keys2, false, null);
	gl.add(g2);

	gl.remove(g1);

	assert (g2.keys.is_equal(gl.valid_keys));

	gl.remove(g2);
	assert(gl.valid_keys.size() == 0);
}

class Tester {
	public int subscribe_count = 0;
	public void callback (bool subscribe) {
		if (subscribe) {
			subscribe_count++;
		} else {
			subscribe_count--;
		}
	}
}

void test_grouplist_subscribe() {
	GroupList gl = new GroupList();

	string[] keys = {"a","b","c"};
	Tester t1 = new Tester();
	Group g1 = new Group(keys, false, t1.callback);
	gl.add(g1);

	string[] keys2 = {"d","f","c"};
	Tester t2 = new Tester();
	Group g2 = new Group(keys2, false, t2.callback);
	gl.add(g2);

	StringSet keyset1 = new StringSet.from_array({"a","b","f"});
	StringSet keyset2 = new StringSet.from_array({"d","c"});
	StringSet keyset3 = new StringSet.from_array({"d","f","c"});
	StringSet subscribedkeys = new StringSet();
	subscribedkeys = new StringSet.union(subscribedkeys, keyset1);
	gl.first_subscribed(null, keyset1); //"a","b","f"
	assert (t1.subscribe_count == 1);
	assert (t2.subscribe_count == 1);
	subscribedkeys = new StringSet.union(subscribedkeys, keyset2);
	gl.first_subscribed(null, keyset2); // "a","b","c","d","f"
	assert (t1.subscribe_count == 1);
	assert (t2.subscribe_count == 1);
	subscribedkeys = new StringSet.difference(subscribedkeys, keyset3);
	gl.last_unsubscribed(null, keyset3, subscribedkeys); // "a","b"
	assert (t1.subscribe_count == 1);
	assert (t2.subscribe_count == 0);
	subscribedkeys = new StringSet.union(subscribedkeys, keyset3);
	gl.first_subscribed(null, keyset3); // "a","b","c","d","f"
	assert (t1.subscribe_count == 1);
	assert (t2.subscribe_count == 1);
	subscribedkeys = new StringSet.difference(subscribedkeys, keyset2);
	gl.last_unsubscribed(null, keyset2, subscribedkeys); // "a", "b", "f"
	assert (t1.subscribe_count == 1);
	assert (t2.subscribe_count == 1);
	subscribedkeys = new StringSet.difference(subscribedkeys, keyset1);
	gl.last_unsubscribed(null, keyset1, subscribedkeys); //empty
	assert (t1.subscribe_count == 0);
	assert (t2.subscribe_count == 0);
}



public static void main (string[] args) {
	Test.init (ref args);
	Test.add_func("/contextkit/grouplist/add", test_grouplist_add);
	Test.add_func("/contextkit/grouplist/remove", test_grouplist_remove);
	Test.add_func("/contextkit/grouplist/subscribe", test_grouplist_subscribe);
	Test.run ();
}

