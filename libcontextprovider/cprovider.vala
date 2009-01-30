using GLib;

namespace ContextProvider {

	internal class CProvider : GLib.Object, ContextProvider.Provider {

		// Stored callbacks
		private GetCallback get_cb;
		private SubscribeCallback first_cb;
		private SubscribeCallback last_cb;

		public CProvider (GetCallback get_cb, SubscribeCallback first_cb, SubscribeCallback last_cb) {
			this.get_cb = get_cb;
			this.first_cb = first_cb;
			this.last_cb = last_cb;
		}

		// Implementation of the Provider interface by using callbacks

		public void Get (StringSet keys, HashTable<string, Value?> ret, ref GLib.List<string> unavail) {
			if (get_cb != null) {
				ChangeSet change_set = new ChangeSet.from_get(ret);
				get_cb (keys, change_set);
				/*UGLY UGLY UGLY. GList sucks in vala. must write Gee.LinkedList*/
				foreach (var s in change_set.undeterminable_keys) {
					unavail.prepend(s);
				}
			}
		}

		public void KeysSubscribed (StringSet keys) {
			if (first_cb != null) {
				first_cb (keys);
			}
		}

		public void KeysUnsubscribed (StringSet keys) {
			if (last_cb != null) {
				last_cb (keys);
			}
		}
	}
}
