using GLib;
using DBus;

namespace ContextKit {

	[DBus (name = "org.freedesktop.ContextKit.Manager")]
	public interface DBusManager : GLib.Object {
		public abstract void Get (string[] keys, out HashTable<string, Value?> values, out string[] undeterminable_keys);
		public abstract DBus.ObjectPath GetSubscriber (DBus.BusName name) throws DBus.Error;
	}

	[DBus (name = "org.freedesktop.ContextKit.Subscriber")]
	public interface DBusSubscriber: GLib.Object {
		public abstract void Subscribe (string[] keys, out HashTable<string, Value?> values, out string[] undeterminable_keys);

		public abstract void Unsubscribe (string[] keys);
		public signal void Changed (HashTable<string, Value?> values, string[] undeterminable_keys);
	}
}
