/* sqlitevala.vala
 *
 * a vala-ified wrapper for sqlite
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

namespace SqliteVala
{
	errordomain DatabaseError {
		OPEN_FAILED,
		CLOSE_FAILED
	}

	public class Database {
		string db_name;
		bool valid = false;
		internal Sqlite.Database db;

		public Database (string db_name) {
			this.db_name = db_name;
		}

		public Database.in_memory () {
			this.db_name = ":memory:";
		}

		public void open () throws DatabaseError {
			if (valid)
				throw new DatabaseError.OPEN_FAILED("Database already open");

			int rc = Sqlite.Database.open (db_name, out db);
			if (rc == Sqlite.OK)
				valid = true;
			else
				throw new DatabaseError.OPEN_FAILED (db.errmsg());
		}

	}

	public class Query {
		Database db;
		string sql;
		Sqlite.Statement stmt;
		bool valid = true;

		public Query (Database db, string sql) {
			this.db = db;
			this.sql = sql;
			if (db.db.prepare_v2 (sql, (int) sql.length, out stmt) == Sqlite.OK)
				valid = true;
		}

		public Array<Value> get_row () {
			
		}
	}
}

