/* test_grouplist.vala
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

void test_grouplist_add() {
	GroupList gl = new GroupList();

	string[] keys = {"a","b","c"};
	Group g = new Group(keys, null);
	gl.add(g);

	assert (g.keys.is_subset_of(gl.valid_keys));

	string[] keys2 = {"d","f","c"};
	Group g2 = new Group(keys2, null);
	gl.add(g2);

	assert (g2.keys.is_subset_of(gl.valid_keys));
}

void test_grouplist_remove() {
	GroupList gl = new GroupList();

	string[] keys = {"a","b","c"};
	Group g = new Group(keys, null);
	gl.add(g);

	string[] keys2 = {"d","f","c"};
	Group g2 = new Group(keys2, null);
	gl.add(g2);

	gl.remove(g);

	assert (g2.keys.is_equal(gl.valid_keys));

	gl.remove(g2);
	assert(gl.valid_keys.size() == 0);
}


public static void main (string[] args) {
       Test.init (ref args);
       Test.add_func("/contextkit/grouplist/add", test_grouplist_add);
       Test.add_func("/contextkit/grouplist/remove", test_grouplist_remove);
       Test.run ();
}

