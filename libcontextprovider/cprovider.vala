using GLib;
using Gee;

namespace ContextProvider {

	internal class CProvider : GLib.Object, ContextProvider.Provider {

		// Stored callbacks
		private GetCallback get_cb;
		private SubscribedCallback first_cb;
		private UnsubscribedCallback last_cb;
		private StringSet keys;

		public CProvider (StringSet keys, GetCallback get_cb, SubscribedCallback first_cb, UnsubscribedCallback last_cb) {
			this.get_cb = get_cb;
			this.first_cb = first_cb;
			this.last_cb = last_cb;
			this.keys = keys;
		}

		// Implementation of the Provider interface by using callbacks

		public void get (StringSet keys, HashTable<string, Value?> ret, ArrayList<string> unavail) {
			debug("CProvider.get called with keys %s", keys.debug());
			if (get_cb != null) {
				ChangeSet change_set = new ChangeSet.from_get(ret, unavail);
				get_cb (keys, change_set);
			}
		}

		public void keys_subscribed (StringSet keys) {
			if (first_cb != null) {
				first_cb (keys);
			}
		}

		public void keys_unsubscribed (StringSet keys_unsubscribed, StringSet keys_remaining) {
			if (last_cb != null) {
				last_cb (keys_unsubscribed, keys_remaining);
			}
		}

		public StringSet provided_keys () {
			return keys;
		}
	}
}
