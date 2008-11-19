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

enum TestValues {
	A = 1,
	B = 2,
	C = 3,
	D = 10000,
	E = 10001
}

void test_intset_add () {
	ContextKit.IntSet intset = new ContextKit.IntSet();
	intset.add (A);
	intset.add (B);
	intset.is_member (A);
	intset.is_member (B);
}

public static void main (string[] args) {
	Test.init (ref args);
	Test.add_func("/contextkit/intset/intset_add", test_store_new);
	Test.run ();
}
