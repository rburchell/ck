/* test_intset.vala
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

void test_intset_sized_new () {
	IntSet intset = new IntSet.sized(0);
	assert (intset.dump() == "");
	IntSet intset2 = new IntSet.sized(10);
	assert (intset2.dump() == "");
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

void test_intset_remove() {
	IntSet intset = new IntSet();
	intset.add (TestValues.A);
	intset.add (TestValues.B);
	intset.add (TestValues.C);
	intset.add (TestValues.D);
	intset.add (TestValues.E);
	assert (intset.remove (TestValues.B) == true);
	assert (intset.is_member (TestValues.A));
	assert (!intset.is_member (TestValues.B));
	assert (intset.is_member (TestValues.C));
	assert (intset.is_member (TestValues.D));
	assert (intset.is_member (TestValues.E));

	//check error cases
	assert (intset.remove (1234) == false);
	assert (intset.remove (1234567) == false);

	//remove everything
	assert (intset.remove (TestValues.A) == true);
	assert (intset.remove (TestValues.C) == true);
	assert (intset.remove (TestValues.D) == true);
	assert (intset.remove (TestValues.E) == true);

	assert(intset.size() == 0);

	//remove something already removed
	assert(intset.remove (TestValues.A) == false);
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

	//and the opposite way around..
	IntSet i4 = new IntSet.intersection (i2, i1);

	assert (i4.is_member (TestValues.C));
	assert (!i4.is_member (TestValues.A));
	assert (!i4.is_member (TestValues.B));
	assert (!i4.is_member (TestValues.D));
	assert (!i4.is_member (TestValues.E));

}

void test_intset_union() {
	IntSet i1 = new IntSet();
	IntSet i2 = new IntSet();
	i1.add (TestValues.A);
	i1.add (TestValues.B);
	i1.add (TestValues.C);
	i2.add (TestValues.C);
	i2.add (TestValues.D);
	i2.add (TestValues.E);

	IntSet i3 = new IntSet.union (i1, i2);

	assert (i3.is_member (TestValues.C));
	assert (i3.is_member (TestValues.A));
	assert (i3.is_member (TestValues.B));
	assert (i3.is_member (TestValues.D));
	assert (i3.is_member (TestValues.E));

	//and the opposite way around..
	IntSet i4 = new IntSet.union (i2, i1);

	assert (i4.is_member (TestValues.C));
	assert (i4.is_member (TestValues.A));
	assert (i4.is_member (TestValues.B));
	assert (i4.is_member (TestValues.D));
	assert (i4.is_member (TestValues.E));

}

void test_intset_symmetric_difference() {
	IntSet i1 = new IntSet();
	IntSet i2 = new IntSet();
	i1.add (TestValues.A);
	i1.add (TestValues.B);
	i1.add (TestValues.C);
	i2.add (TestValues.C);
	i2.add (TestValues.D);
	i2.add (TestValues.E);

	IntSet i3 = new IntSet.symmetric_difference (i1, i2);

	assert (!i3.is_member (TestValues.C));
	assert (i3.is_member (TestValues.A));
	assert (i3.is_member (TestValues.B));
	assert (i3.is_member (TestValues.D));
	assert (i3.is_member (TestValues.E));

	//and the opposite way around..
	IntSet i4 = new IntSet.symmetric_difference (i2, i1);

	assert (!i4.is_member (TestValues.C));
	assert (i4.is_member (TestValues.A));
	assert (i4.is_member (TestValues.B));
	assert (i4.is_member (TestValues.D));
	assert (i4.is_member (TestValues.E));

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

void test_intset_is_subset_of() {
	IntSet i1 = new IntSet();
	IntSet i2 = new IntSet();
	i1.add (TestValues.A);
	i1.add (TestValues.B);
	i1.add (TestValues.C);
	i2.add (TestValues.B);
	i2.add (TestValues.C);

	assert (!i1.is_subset_of(i2));
	assert (i2.is_subset_of(i1));

	i2.add (TestValues.D);

	assert (!i2.is_subset_of(i1));
}

void test_intset_is_equal() {
	IntSet i1 = new IntSet();
	IntSet i2 = new IntSet();
	i1.add (TestValues.A);
	i1.add (TestValues.B);
	i1.add (TestValues.C);
	i2.add (TestValues.A);
	i2.add (TestValues.B);
	i2.add (TestValues.C);

	assert (i1.is_equal(i2));
	assert (i2.is_equal(i1));

	i2.add (TestValues.D);

	assert (!i2.is_equal(i1));
	assert (!i1.is_equal(i2));
}

void test_intset_dump() {
	IntSet intset = new IntSet();
	intset.add (TestValues.A);
	intset.add (TestValues.B);
	intset.add (TestValues.C);
	intset.add (TestValues.D);
	intset.add (TestValues.E);
	assert (intset.dump() == "1 2 3 10000 10001");
}

public static void main (string[] args) {

	Test.init (ref args);
	Test.add_func("/contextkit/intset/new", test_intset_new);
	Test.add_func("/contextkit/intset/sized_new", test_intset_sized_new);
	Test.add_func("/contextkit/intset/add", test_intset_add);
	Test.add_func("/contextkit/intset/remove", test_intset_remove);
	Test.add_func("/contextkit/intset/intersect", test_intset_intersect);
	Test.add_func("/contextkit/intset/union", test_intset_union);
	Test.add_func("/contextkit/intset/symmetric_difference", test_intset_symmetric_difference);
	Test.add_func("/contextkit/intset/disjoint", test_intset_disjoint);
	Test.add_func("/contextkit/intset/is_subset_of", test_intset_is_subset_of);
	Test.add_func("/contextkit/intset/is_equal", test_intset_is_equal);
	Test.add_func("/contextkit/intset/dump", test_intset_dump);
	Test.run ();
}
