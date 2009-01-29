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
			ChangeSet change_set = ChangeSet.create();
			get_cb (keys, change_set);
			ChangeSet.commit (change_set);
		}

		public void KeysSubscribed (StringSet keys) {
			first_cb (keys);
		}

		public void KeysUnsubscribed (StringSet keys) {
			last_cb (keys);
		}
	}
}
