using GLib;

namespace ContextKit {

	public TypedVariant make_typed_variant (ValueType type, Value value) {
		TypedVariant tv = TypedVariant();
		tv.type = type;
		tv.value.init (value.type());
		value.copy (ref tv.value);
		return tv;
	}

	public struct Key {
		public string name;
		public ValueType type;
	}

	public interface PluginSubscription : GLib.Object {
		public abstract void Unsubscribe ();
		public signal void Changed (HashTable<string, TypedVariant?> values);
	}

	public interface Plugin : GLib.Object {
		public abstract void Get (StringSet keys, HashTable<string, TypedVariant?> ret);
		public abstract PluginSubscription Subscribe (StringSet keys, out HashTable<string, TypedVariant?> values);
		public abstract Key[] Keys();
	}
}
