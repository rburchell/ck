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

	debug (g.keys.debug());
	debug (gl.valid_keys.debug());
	assert (g.keys.is_subset_of(gl.valid_keys));

	string[] keys2 = {"d","f","c"};
	Group g2 = new Group(keys2, null);
	gl.add(g2);

	debug (g2.keys.debug());
	debug (gl.valid_keys.debug());
	assert (g2.keys.is_subset_of(gl.valid_keys));
}


public static void main (string[] args) {
       Test.init (ref args);
       Test.add_func("/contextkit/grouplist/register", test_grouplist_add);
       Test.run ();
}

