using GLib;

namespace ContextProvider {
	public interface Provider : GLib.Object {
		public abstract void Get (StringSet keys, HashTable<string, Value?> ret, ref List<string> unavailable_keys);
		public abstract void KeysSubscribed (StringSet keys);
		public abstract void KeysUnsubscribed (StringSet keys);
	}
}
