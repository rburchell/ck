using GLib;

namespace ContextKit {

	public struct Key {
		public string name;
		public ValueType type;
	}

	public interface Plugin : GLib.Object {
		public abstract void Get (StringSet keys, HashTable<string, TypedVariant?> ret);
		public abstract void Subscribe (StringSet keys, ContextKit.Subscription s);
		public abstract Key[] Keys();
	}

	namespace PluginMixins {

		public class SubscriptionList <T> : GLib.Object {
			T parent;
			public delegate void Removed (T parent, Subscription s);

			public SubscriptionList (T parent, Removed callback) {
				this.parent = parent;
				this.callback = callback;
			}

			public void add (Subscription s) {
				s.weak_ref ((WeakNotify)weak_notify, this);
				list.add (s);
			}

			public void remove (Subscription s) {
				s.weak_unref ((WeakNotify)weak_notify, this);
				list.remove (s);
			}

			void weak_notify (Object obj) {
				weak Subscription s = (Subscription) obj;
				list.remove (s);
				callback (parent, s);
			}

			Gee.ArrayList<weak Subscription> list;
			Removed callback;
		}
	}
}
