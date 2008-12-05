using GLib;

namespace ContextKit {

	public struct Key {
		public string name;
		public ValueType type;
	}

	public interface Plugin : GLib.Object {
		public abstract void Get (StringSet keys, HashTable<string, TypedVariant?> ret);
		public abstract void Subscribe (StringSet keys, ContextKit.Subscriber s);
		public abstract Key[] Keys();
	}

	namespace PluginMixins {

		public class SubscriberList <T> : GLib.Object {
			T parent;
			public delegate void Removed (T parent, Subscriber s);

			public SubscriberList (T parent, Removed callback) {
				this.parent = parent;
				this.callback = callback;
			}

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

			void weak_notify (Object obj) {
				weak Subscriber s = (Subscriber) obj;
				list.remove (s);
				callback (parent, s);
			}
			Gee.ArrayList<weak Subscriber> list;
			Removed callback;
		}
	}
}
