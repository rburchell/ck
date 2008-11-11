namespace ContextKit {
	public class MCE : GLib.Object, Plugin {
		DBus.Connection conn;
		dynamic DBus.Object mce_request;
		dynamic DBus.Object mce_signal;

		public MCE () {
			conn = DBus.Bus.get (DBus.BusType.SYSTEM);
			mce_request = conn.get_object ("com.nokia.mce", "/com/nokia/mce/request", "com.nokia.mce.request");
			mce_signal = conn.get_object ("com.nokia.mce", "/com/nokia/mce/signal", "com.nokia.mce.signal");
		}
		public HashTable<string, TypedVariant?> Get (string[] keys)
			{}
		public PluginSubscription Subscribe (string[] keys, out HashTable<string, TypedVariant?> values);
		public string[] Keys() {
			return {"Context.Device.Orientation.displayFacingUp",
				"Context.Device.Orientation.displayFacingDown",
				"Context.Device.Orientation.inLandscape",
				"Context.Device.Orientation.inPortrait"}
		}
	}
}
