namespace ContextKit {

	public struct Key {
		string name;
		ValueType type;
	}

	public interface PluginSubscription : GLib.Object {
		public abstract void Unsubscribe ();
		public signal void Changed (HashTable<string, TypedVariant?> values);
	}

	public interface Plugin : GLib.Object {
		public abstract HashTable<string, TypedVariant?> Get (string[] keys);
		public abstract PluginSubscription Subscribe (string[] keys, out HashTable<string, TypedVariant?> values);
		public abstract Key[] Keys();
	}
}
