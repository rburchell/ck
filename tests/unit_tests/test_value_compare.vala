/* value_compare.vala
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

void test_value_compare_null() {
	Value? v1 = null;
	Value? v2 = null;
	assert(value_compare(v1,v2));
	v1 = Value (typeof(int));
	v1.set_int (12);
	assert(!value_compare(v1,v2));
	v2 = v1;
	v1 = null;
	assert(!value_compare(v1,v2));
}

void test_value_compare_int() {
	Value v1 = Value (typeof(int));
	v1.set_int (12);
	Value v2 = Value (typeof(int));
	v2.set_int (12);
	assert(value_compare(v1,v2));

	v2.set_int (24);
	assert(!value_compare(v1,v2));
}

void test_value_compare_bool() {
	Value v1 = Value (typeof(bool));
	v1.set_boolean (false);
	Value v2 = Value (typeof(bool));
	v2.set_boolean (true);
	assert(!value_compare(v1,v2));

	v2.set_boolean (false);
	assert(value_compare(v1,v2));
}

void test_value_compare_double() {
	Value v1 = Value (typeof(double));
	v1.set_double (245.42);
	Value v2 = Value (typeof(double));
	v2.set_double (245.42);
	assert(value_compare(v1,v2));

	v2.set_double (12345.98723);
	assert(!value_compare(v1,v2));
}

void test_value_compare_string() {
	Value v1 = Value (typeof(string));
	v1.set_string ("foo bar baz");
	Value v2 = Value (typeof(string));
	v2.set_string ("buz bong fu");
	assert(!value_compare(v1,v2));

	v2.set_string ("foo bar baz");
	assert(value_compare(v1,v2));
}



public static void main (string[] args) {
	Test.init (ref args);
	Test.add_func("/contextkit/value_compare/null", test_value_compare_null);
	Test.add_func("/contextkit/value_compare/int", test_value_compare_int);
	Test.add_func("/contextkit/value_compare/bool", test_value_compare_bool);
	Test.add_func("/contextkit/value_compare/double", test_value_compare_double);
	Test.add_func("/contextkit/value_compare/string", test_value_compare_string);
	Test.run ();
}

