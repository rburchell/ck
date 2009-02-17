using GLib;
using Gee;

namespace ContextProvider {
	/**
	 * ContextProviderProvider:
	 *
	 * Represents a provider of context
	 */
	public interface Provider : GLib.Object {
		public abstract void get (StringSet keys, HashTable<string, Value?> ret, ArrayList<string> unavailable_keys);
		public abstract void keys_subscribed (StringSet keys_subscribed);
		public abstract void keys_unsubscribed (StringSet keys_unsubscribed, StringSet keys_left);
		public abstract StringSet provided_keys();
	}
}
