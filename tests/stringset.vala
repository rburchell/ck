/* main.vala
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
using ContextKit;

void test_stringset_new () {
	StringSet stringset = new StringSet();
	assert (!stringset.is_member ("A"));
}

void test_stringset_new_from_array () {
	string [] vals  = { "A","B","C","D" };
	StringSet stringset = new StringSet.from_array(vals);
	assert (stringset.is_member ("A"));
	assert (stringset.is_member ("B"));
	assert (stringset.is_member ("C"));
	assert (stringset.is_member ("D"));
	assert (!stringset.is_member ("E"));
}

void test_stringset_add () {
	StringSet stringset = new StringSet();
	stringset.add ("A");
	stringset.add ("B");
	stringset.add ("C");
	stringset.add ("D");
	stringset.add ("E");
	assert (stringset.is_member ("A"));
	assert (stringset.is_member ("B"));
	assert (stringset.is_member ("C"));
	assert (stringset.is_member ("D"));
	assert (stringset.is_member ("E"));
}

void test_stringset_intersect () {
	StringSet i1 = new StringSet();
	StringSet i2 = new StringSet();
	i1.add ("A");
	i1.add ("B");
	i1.add ("C");
	i2.add ("C");
	i2.add ("D");
	i2.add ("E");

	StringSet i3 = new StringSet.intersection (i1, i2);

	assert (i3.is_member ("C"));
	assert (!i3.is_member ("A"));
	assert (!i3.is_member ("B"));
	assert (!i3.is_member ("D"));
	assert (!i3.is_member ("E"));
}

void test_stringset_disjoint () {
	StringSet i1 = new StringSet();
	StringSet i2 = new StringSet();
	i1.add ("A");
	i1.add ("B");
	i1.add ("C");
	i2.add ("C");
	i2.add ("D");
	i2.add ("E");

	assert (!i1.is_disjoint (i2));

	i1.remove ("C");
	assert (i1.is_disjoint (i2));
}

void test_stringset_to_array () {
	StringSet stringset = new StringSet();
	stringset.add ("A");
	stringset.add ("B");
	stringset.add ("C");
	stringset.add ("D");
	stringset.add ("E");

	Gee.ArrayList<string> array = stringset.to_array();
	assert (array.get(0) == "A");
	assert (array.get(1) == "B");
	assert (array.get(2) == "C");
	assert (array.get(3) == "D");
	assert (array.get(4) == "E");
}



public static void main (string[] args) {

	Test.init (ref args);
	Test.add_func("/contextkit/stringset/new", test_stringset_new);
	Test.add_func("/contextkit/stringset/new_from_array", test_stringset_new_from_array);
	Test.add_func("/contextkit/stringset/add", test_stringset_add);
	Test.add_func("/contextkit/stringset/intersect", test_stringset_intersect);
	Test.add_func("/contextkit/stringset/disjoint", test_stringset_disjoint);
	Test.add_func("/contextkit/stringset/to_array", test_stringset_to_array);
	Test.run ();
}
