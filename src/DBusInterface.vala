using GLib;
using DBus;

namespace ContextKit {

	enum ValueType {
		STRING,
		INTEGER,
		TRUTH
	}

	struct TypedVariant {
		ValueType type;
		Value value;
	}

	[DBus (name = "org.freedesktop.ContextKit.Manager")]
	public interface IDBusManager {
		public abstract HashTable<string, TypedVariant> Get (string[] keys);
		public abstract ObjectPath Subscribe (string[] keys, out HashTable<string, TypedVariant> values);
	}

	[DBus (name = "org.freedesktop.ContextKit.Subscription")]
	public interface IDBusSubscription {
		public abstract HashTable<string, TypedVariant> Get (string[] keys);
		public abstract ObjectPath Subscribe (string[] keys, out HashTable<string, TypedVariant> values);
	}


}	
