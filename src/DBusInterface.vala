using GLib;
using DBus;

namespace ContextKit {

	public enum ValueType {
		STRING,
		INTEGER,
		DOUBLE,
		TRUTH,
		UNDETERMINED
	}

	public struct TypedVariant {
		public TypedVariant (ValueType type, Value value) {
			this.type = type;
			this.value.init (value.type());
			value.copy (ref this.value);
		}
		public ValueType type;
		public Value value;
	}
/*
	[DBus (name = "org.freedesktop.ContextKit.Manager")]
	public interface DBusManager : GLib.Object {
		public abstract HashTable<string, TypedVariant?> Get (string[] keys);
		public abstract ObjectPath Subscribe (string[] keys, out HashTable<string, TypedVariant?> values);
	}

	[DBus (name = "org.freedesktop.ContextKit.Subscription")]
	public interface DBusSubscription : GLib.Object {
		public abstract void Unsubscribe ();
		public signal void Changed (HashTable<string, TypedVariant?> values);
	}
	*/
}
