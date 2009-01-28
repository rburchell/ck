using GLib;

namespace ContextKit {
	namespace CProvider {
		public class Provider : GLib.Object, ContextKit.Provider {
			
			// Stored callbacks
			private Context.get_callback get_cb;
			private Context.subscribe_callback first_cb;
			private Context.subscribe_callback last_cb;

			public Provider (Context.get_callback get_cb, Context.subscribe_callback first_cb, Context.subscribe_callback last_cb) {
				this.get_cb = get_cb;
				this.first_cb = first_cb;
				this.last_cb = last_cb;
			}

			// Implementation of the Provider interface by using callbacks

			public void Get (StringSet keys, HashTable<string, Value?> ret, ref List<string> unavail) {
				Context.ChangeSet changeSet = new Context.ChangeSet();
				get_cb (keys, changeSet);
			}

			public void KeysSubscribed (StringSet keys) {
				first_cb (keys);
			}

			public void KeysUnsubscribed (StringSet keys) {
				last_cb (keys);
			}
		}
	}
}
