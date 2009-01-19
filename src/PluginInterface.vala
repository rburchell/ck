using GLib;

namespace ContextKit {

	public struct Key {
		public string name;
	}

	public interface Provider : GLib.Object {
		public abstract void Get (StringSet keys, HashTable<string, Value?> ret, ref List<string> unavailable_keys);
		public abstract void KeysSubscribed (StringSet keys);
		public abstract void KeysUnsubscribed (StringSet keys);
	}

	namespace ProviderMixins {

		/*TODO: a generalisation of this should probably be submitted for libgee*/
		public class SubscriberList : GLib.Object {

			public void add (Subscriber s) {
				s.weak_ref ((WeakNotify)weak_notify, this);
				list.add (s);
			}

			public void remove (Subscriber s) {
				s.weak_unref ((WeakNotify)weak_notify, this);
				list.remove (s);
			}

			public bool contains (Subscriber s) {
				return list.contains (s);
			}

			public int size {
				get { return list.size; }
			}

			public weak Subscriber get (int i) {
				return list[i];
			}

			public signal void removed (Subscriber s);

			void weak_notify (Object obj) {
				weak Subscriber s = (Subscriber) obj;
				list.remove (s);
				removed (s);
			}

			Gee.ArrayList<weak Subscriber> list = new Gee.ArrayList<weak Subscriber>();
		}
	}
}
