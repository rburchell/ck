using GLib;
using DBus;

namespace ContextKit {

	public enum ValueType {
    UNDETERMINED,
		STRING,
		INTEGER,
		DOUBLE,
		TRUTH
	}

	[DBus (name = "org.freedesktop.ContextKit.Manager")]
	public interface DBusManager : GLib.Object {
		public abstract void Get (string[] keys, out HashTable<string, Value?> values, out string[] undeterminable_keys);
		public abstract DBus.ObjectPath GetSubscriber (DBus.BusName name) throws DBus.Error;
	}

/*
	[DBus (name = "org.freedesktop.ContextKit.Subscription")]
	public interface DBusSubscription : GLib.Object {
		public abstract void Unsubscribe ();
		public signal void Changed (HashTable<string, TypedVariant?> values);
	}
	*/
}
