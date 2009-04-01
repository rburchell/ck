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
	Group g = new Group(keys, null);
	StringSet keyset = new StringSet.from_array(keys);
	assert (keyset.is_equal(g.keys));
}


public static void main (string[] args) {
       Test.init (ref args);
       Test.add_func("/contextkit/group/keys", test_group_keys);
       Test.run ();
}

