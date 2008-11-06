using GLib;
using DBus;

namespace ContextKit {

	public enum ValueType {
		STRING,
		INTEGER,
		TRUTH
	}

	public struct TypedVariant {
		public ValueType type;
		public Value value;
	}

	[DBus (name = "org.freedesktop.ContextKit.Manager")]
	public interface IDBusManager {
		public abstract HashTable<string, TypedVariant?> Get (string[] keys);
		public abstract ObjectPath Subscribe (string[] keys, out HashTable<string, TypedVariant?> values);
	}

	[DBus (name = "org.freedesktop.ContextKit.Subscription")]
	public interface IDBusSubscription {
		public abstract HashTable<string, TypedVariant?> Get (string[] keys);
		public abstract ObjectPath Subscribe (string[] keys, out HashTable<string, TypedVariant?> values);
	}
}	
