namespace ContextKit {

	namespace MCE {
		struct DeviceOrientation {
			public string rotation;
			public string stand;
			public string facing;
			public int x;
			public int y;
			public int z;
		}

		public class Subscription: GLib.Object, ContextKit.PluginSubscription {
			public void Unsubscribe () {
			}
		}

		public class Plugin : GLib.Object, ContextKit.Plugin {
			DBus.Connection conn;
			dynamic DBus.Object mce_request;
			dynamic DBus.Object mce_signal;

			const string[] keys = {
					"Context.Device.Orientation.displayFacingUp",
					"Context.Device.Orientation.displayFacingDown",
					"Context.Device.Orientation.inLandscape",
					"Context.Device.Orientation.inPortrait"
			};
	
			public Plugin () {
				conn = DBus.Bus.get (DBus.BusType.SYSTEM);
				mce_request = conn.get_object ("com.nokia.mce", "/com/nokia/mce/request", "com.nokia.mce.request");
				mce_signal = conn.get_object ("com.nokia.mce", "/com/nokia/mce/signal", "com.nokia.mce.signal");
			}

			public void Get (string[] keys, HashTable<string, TypedVariant?> ret) {
				DeviceOrientation orientation;
				try {
					mce_request.get_device_orientation(out orientation);
				} catch (GLib.Error ex) {
					stdout.printf("MCE Plugin Error: %s\n", ex.message);
					return;
				}

				foreach (string key in keys) {
					switch (key) {
						case "Context.Device.Orientation.displayFacingUp" :
							if ( orientation.facing == "face_up" ) {
								/* TODO: clean this code when vala bug 528436 lands */
								Value truth;
								truth.init (typeof(bool));
								truth.set_boolean (true);
								TypedVariant tv = TypedVariant () {type = ValueType.TRUTH, value = truth};
								ret.insert (key, tv);
							}
						break;
						case "Context.Device.Orientation.displayFacingDown" :
							if ( orientation.facing == "face_down" ) {
								/* TODO: clean this code when vala bug 528436 lands */
								Value truth;
								truth.init (typeof(bool));
								truth.set_boolean (true);
								TypedVariant tv = TypedVariant () {type = ValueType.TRUTH, value = truth};
								ret.insert (key, tv);
							}
						break;
						case "Context.Device.Orientation.inPortrait" :
							if ( orientation.rotation == "portrait" ) {
								/* TODO: clean this code when vala bug 528436 lands */
								Value truth;
								truth.init (typeof(bool));
								truth.set_boolean (true);
								TypedVariant tv = TypedVariant () {type = ValueType.TRUTH, value = truth};
								ret.insert (key, tv);
							}
						break;
						case "Context.Device.Orientation.inPortrait" :
							if ( orientation.rotation == "portrait" ) {
								/* TODO: clean this code when vala bug 528436 lands */
								Value truth;
								truth.init (typeof(bool));
								truth.set_boolean (true);
								TypedVariant tv = TypedVariant () {type = ValueType.TRUTH, value = truth};
								ret.insert (key, tv);
							}
						break;
					}
				}
			}

			public PluginSubscription Subscribe (string[] keys, out HashTable<string, TypedVariant?> values) {
				return new Subscription();
			}

			public string[] Keys() {
			return keys;
			}
		}
	}
}
