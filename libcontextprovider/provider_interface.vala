using GLib;

namespace ContextProvider {
	public interface Provider : GLib.Object {
		public abstract void get (StringSet keys, HashTable<string, Value?> ret, ref List<string> unavailable_keys);
		public abstract void keys_subscribed (StringSet keys);
		public abstract void keys_unsubscribed (StringSet keys);
		public abstract StringSet provided_keys();
	}
}
