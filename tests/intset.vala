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

enum TestValues {
	A = 1,
	B = 2,
	C = 3,
	D = 10000,
	E = 10001
}

void test_intset_new () {
	IntSet intset = new IntSet();
	assert (intset.dump() == "");
}

void test_intset_add () {
	IntSet intset = new IntSet();
	intset.add (TestValues.A);
	intset.add (TestValues.B);
	intset.add (TestValues.C);
	intset.add (TestValues.D);
	intset.add (TestValues.E);
	assert (intset.is_member (TestValues.A));
	assert (intset.is_member (TestValues.B));
	assert (intset.is_member (TestValues.C));
	assert (intset.is_member (TestValues.D));
	assert (intset.is_member (TestValues.E));
}

void test_intset_intersect () {
	IntSet i1 = new IntSet();
	IntSet i2 = new IntSet();
	i1.add (TestValues.A);
	i1.add (TestValues.B);
	i1.add (TestValues.C);
	i2.add (TestValues.C);
	i2.add (TestValues.D);
	i2.add (TestValues.E);

	IntSet i3 = new IntSet.intersection (i1, i2);

	assert (i3.is_member (TestValues.C));
	assert (!i3.is_member (TestValues.A));
	assert (!i3.is_member (TestValues.B));
	assert (!i3.is_member (TestValues.D));
	assert (!i3.is_member (TestValues.E));
}

void test_intset_disjoint () {
	IntSet i1 = new IntSet();
	IntSet i2 = new IntSet();
	i1.add (TestValues.A);
	i1.add (TestValues.B);
	i1.add (TestValues.C);
	i2.add (TestValues.C);
	i2.add (TestValues.D);
	i2.add (TestValues.E);

	assert (!i1.is_disjoint (i2));

	i1.remove (TestValues.C);
	assert (i1.is_disjoint (i2));
}



public static void main (string[] args) {

	Test.init (ref args);
	Test.add_func("/contextkit/intset/new", test_intset_new);
	Test.add_func("/contextkit/intset/add", test_intset_add);
	Test.add_func("/contextkit/intset/intersect", test_intset_intersect);
	Test.add_func("/contextkit/intset/disjoint", test_intset_disjoint);
	Test.run ();
}
