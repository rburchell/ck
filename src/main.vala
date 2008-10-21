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
using Sqlite;

public class SqliteSample : Object {

    private string column_names;

    private static string create = """
	CREATE TABLE My_table (
	  field1   INT,
	  field2   VARCHAR (50),
	  field3   INT,
	  PRIMARY KEY (field1, field2)
	  );""";

    private static string insert = "INSERT INTO My_table (field1, field2, field3) VALUES (%d, '%s', %d);";

    private static string select = "SELECT (field3) FROM My_table WHERE field2='%s';";

    public int test () {
        Database db;
        int rc;

        rc = Database.open (":memory:", out db);

        if (rc != Sqlite.OK) {
            stderr.printf ("Can't open database: %d, %s\n", rc, db.errmsg ());
            return 1;
        }

        rc = db.exec (create, (Sqlite.Callback) callback, this);

        if (rc != Sqlite.OK) {
            stderr.printf ("SQL error in '%s': %d, %s\n", create, rc, db.errmsg ());
            return 1;
        }

        rc = db.exec (insert.printf(12, "test", 1), (Sqlite.Callback) callback, null);
        if (rc != Sqlite.OK) {
            stderr.printf ("SQL error in '%s': %d, %s\n", insert, rc, db.errmsg ());
            return 1;
	}

	rc = db.exec (insert.printf(2, "johns undies", 21), (Sqlite.Callback) callback, null);
        if (rc != Sqlite.OK) {
            stderr.printf ("SQL error in '%s': %d, %s\n", insert, rc, db.errmsg ());
            return 1;
	}


        rc = db.exec (select.printf("johns undies"), (Sqlite.Callback) callback, null);
        if (rc != Sqlite.OK) {
            stderr.printf ("SQL error in '%s': %d, %s\n", select, rc, db.errmsg ());

            return 1;
	}

	stdout.printf ("%s\n", column_names);

        return 0;
    }
}

public class Contextd.Main : Object {
	public Main () {
	}

	public void run () {
		stdout.printf ("Hello, world!\n");

		var sample = new SqliteSample ();
		sample.test();
	}

	static int main (string[] args) {
		var main = new Main ();
		main.run ();
		return 0;
	}

}
