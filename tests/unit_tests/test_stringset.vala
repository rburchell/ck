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
using ContextProvider;

void test_stringset_new () {
	StringSet stringset = new StringSet();
	assert (!stringset.is_member ("A"));
}

void test_stringset_new_sized () {
	StringSet stringset = new StringSet.sized(20);
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

void test_stringset_remove () {
	string [] vals  = { "A","B","C","D" };
	StringSet stringset = new StringSet.from_array(vals);
	stringset.remove ("C");
	stringset.remove ("D");
	assert (stringset.is_member ("A"));
	assert (stringset.is_member ("B"));
	assert (!stringset.is_member ("C"));
	assert (!stringset.is_member ("D"));
	assert (!stringset.is_member ("E"));
	//test removal of something we don't know about
	stringset.remove("badger");
}

void test_stringset_clear () {
	string [] vals  = { "A","B","C","D","E" };
	StringSet stringset = new StringSet.from_array(vals);
	stringset.clear ();
	assert (!stringset.is_member ("A"));
	assert (!stringset.is_member ("B"));
	assert (!stringset.is_member ("C"));
	assert (!stringset.is_member ("D"));
	assert (!stringset.is_member ("E"));
}

void test_stringset_size () {
	string [] vals  = { "A","B","C","D" };
	StringSet stringset = new StringSet.from_array(vals);
	assert (stringset.size() == 4);
	stringset.remove ("C");
	stringset.remove ("D");
	assert (stringset.is_member ("A"));
	assert (stringset.is_member ("B"));
	assert (!stringset.is_member ("C"));
	assert (!stringset.is_member ("D"));
}


void test_stringset_intersection () {
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

void test_stringset_to_array () {
	StringSet stringset = new StringSet();
	stringset.add ("A");
	stringset.add ("B");
	stringset.add ("C");
	stringset.add ("D");
	stringset.add ("E");

	Gee.ArrayList<string> array = stringset.to_array();
	assert (array.size == 5);
	assert (array.get(0) == "A");
	assert (array.get(1) == "B");
	assert (array.get(2) == "C");
	assert (array.get(3) == "D");
	assert (array.get(4) == "E");
}

void test_stringset_iterate () {
	string [] vals = {"A", "B", "C", "D", "E"};

	StringSet stringset = new StringSet.from_array(vals);

	foreach (var s in stringset) {
		//debug(s);
		bool found = false;
		foreach (var v in vals) {
			if (s == v) found = true;
		}
		assert (found);
	}
}

void test_stringset_symmetric_difference() {
	StringSet i1 = new StringSet();
	StringSet i2 = new StringSet();
	i1.add ("A");
	i1.add ("B");
	i1.add ("C");
	i2.add ("C");
	i2.add ("D");
	i2.add ("E");

	StringSet i3 = new StringSet.symmetric_difference(i1, i2);

	assert (!i3.is_member ("C"));
	assert (i3.is_member ("A"));
	assert (i3.is_member ("B"));
	assert (i3.is_member ("D"));
	assert (i3.is_member ("E"));
}

void test_stringset_difference() {
	StringSet i1 = new StringSet();
	StringSet i2 = new StringSet();
	i1.add ("A");
	i1.add ("B");
	i1.add ("C");
	i2.add ("C");
	i2.add ("D");
	i2.add ("E");

	StringSet i3 = new StringSet.difference(i1, i2);

	assert (!i3.is_member ("C"));
	assert (i3.is_member ("A"));
	assert (i3.is_member ("B"));
	assert (!i3.is_member ("D"));
	assert (!i3.is_member ("E"));
}

void test_stringset_union() {
	StringSet i1 = new StringSet();
	StringSet i2 = new StringSet();
	i1.add ("A");
	i1.add ("B");
	i1.add ("C");
	i2.add ("C");
	i2.add ("D");
	i2.add ("E");

	StringSet i3 = new StringSet.union(i1, i2);

	assert (i3.is_member ("A"));
	assert (i3.is_member ("B"));
	assert (i3.is_member ("C"));
	assert (i3.is_member ("D"));
	assert (i3.is_member ("E"));
}



void test_stringset_is_equal() {
	string [] vals = {"A", "B", "C", "D", "E"};
	string [] vals1 = {"B", "C", "D", "E"};
	string [] vals2 = {"a", "b", "c", "d", "e"};

	StringSet s1 = new StringSet.from_array(vals);
	StringSet s2 = new StringSet.from_array(vals);
	StringSet s3 = new StringSet.from_array(vals2);
	StringSet s4 = new StringSet.from_array(vals1);

	assert (s1.is_equal(s1));

	assert (s1.is_equal(s2));
	assert (s2.is_equal(s1));
	assert (!s1.is_equal(s3));
	assert (!s3.is_equal(s1));
	assert (!s3.is_equal(s2));
	assert (!s2.is_equal(s3));
	assert (!s4.is_equal(s1));
	assert (!s1.is_equal(s4));
}

void test_stringset_is_subset_of() {
	string [] vals1 = {"A", "B", "C", "D", "E"};
	string [] vals2 = {"B", "C", "D"};
	string [] vals3 = {"a", "b", "c", "d", "e"};

	StringSet s1 = new StringSet.from_array(vals1);
	StringSet s2 = new StringSet.from_array(vals2);
	StringSet s3 = new StringSet.from_array(vals3);

	assert (s2.is_subset_of(s1));
	assert (!s3.is_subset_of(s2));
	assert (!s3.is_subset_of(s1));
	assert (!s1.is_subset_of(s3));
	assert (!s2.is_subset_of(s3));
}

void test_stringset_is_disjoint() {
	string [] vals1 = {"A", "B", "C", "D", "E"};
	string [] vals2 = {"B", "C", "D"};
	string [] vals3 = {"a", "b", "c", "d", "e"};

	StringSet s1 = new StringSet.from_array(vals1);
	StringSet s2 = new StringSet.from_array(vals2);
	StringSet s3 = new StringSet.from_array(vals3);

	assert (!s1.is_disjoint(s1));

	assert (!s1.is_disjoint(s2));
	assert (!s2.is_disjoint(s1));
	assert (s1.is_disjoint(s3));
	assert (s3.is_disjoint(s1));
	assert (s3.is_disjoint(s1));
	assert (s1.is_disjoint(s3));
}


void test_stringset_debug() {
	string [] vals = {"A", "B", "C", "D", "E"};
	StringSet s = new StringSet.from_array(vals);
	assert (s.debug() == "A, B, C, D, E");
}

public static void main (string[] args) {

	Test.init (ref args);
	Test.add_func("/contextkit/stringset/new", test_stringset_new);
	Test.add_func("/contextkit/stringset/new_sized", test_stringset_new_sized);
	Test.add_func("/contextkit/stringset/new_from_array", test_stringset_new_from_array);
	Test.add_func("/contextkit/stringset/add", test_stringset_add);
	Test.add_func("/contextkit/stringset/remove", test_stringset_remove);
	Test.add_func("/contextkit/stringset/clear", test_stringset_clear);
	Test.add_func("/contextkit/stringset/size", test_stringset_size);
	Test.add_func("/contextkit/stringset/intersection", test_stringset_intersection);
	Test.add_func("/contextkit/stringset/union", test_stringset_union);
	Test.add_func("/contextkit/stringset/difference", test_stringset_difference);
	Test.add_func("/contextkit/stringset/symmetric_difference", test_stringset_symmetric_difference);
	Test.add_func("/contextkit/stringset/disjoint", test_stringset_is_disjoint);
	Test.add_func("/contextkit/stringset/to_array", test_stringset_to_array);
	Test.add_func("/contextkit/stringset/iterate", test_stringset_iterate);
	Test.add_func("/contextkit/stringset/is_equal", test_stringset_is_equal);
	Test.add_func("/contextkit/stringset/is_subset_of", test_stringset_is_subset_of);
	Test.add_func("/contextkit/stringset/is_disjoint", test_stringset_is_disjoint);
	Test.add_func("/contextkit/stringset/debug", test_stringset_debug);
	Test.run ();
}