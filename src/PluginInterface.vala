using GLib;

namespace ContextKit {

	public struct Key {
		public string name;
	}

	public class PluginStruct : GLib.Object {
		// A new key was subscribed to for the first time
		public subscribe_callback first_subscribed_cb;
		// A key is not anymore subscribed to by anyone
		public subscribe_callback last_unsubscribed_cb;
		// A Get request is executed
		public get_callback get_cb;
	}

	public interface Plugin : GLib.Object {
		public abstract void Get (StringSet keys, HashTable<string, Value?> ret, List<string> unavailable_keys);
		public abstract void Subscribe (StringSet keys, ContextKit.Subscriber s);
		public abstract Key[] Keys();
		
	}

	namespace PluginMixins {

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
