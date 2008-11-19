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

			delegate void TruthFunction (void* data);
			const Key[] keys = {
					{
						"Context.Device.Orientation.displayFacingUp",
						ValueType.TRUTH
					},
					{
						"Context.Device.Orientation.displayFacingDown",
						ValueType.TRUTH
					},
					{
						"Context.Device.Orientation.inLandscape",
						ValueType.TRUTH
					},
					{
						"Context.Device.Orientation.inPortrait",
						ValueType.TRUTH
					}
				};

			public Plugin () {
				conn = DBus.Bus.get (DBus.BusType.SYSTEM);
				mce_request = conn.get_object ("com.nokia.mce", "/com/nokia/mce/request", "com.nokia.mce.request");
				mce_signal = conn.get_object ("com.nokia.mce", "/com/nokia/mce/signal", "com.nokia.mce.signal");
			}

			public void Get (string[] keys, HashTable<string, TypedVariant?> ret) {
				DeviceOrientation orientation = DeviceOrientation ();
				try {
					mce_request.get_device_orientation(out orientation.rotation, out orientation.stand, out orientation.facing, out orientation.x, out orientation.y, out orientation.z);
				} catch (GLib.Error ex) {
					stdout.printf("MCE Plugin Error: %s\n", ex.message);
					return;
				}

				stdout.printf("got orientation %s %s %s (%d,%d,%d)", orientation.rotation,  orientation.stand,  orientation.facing,  orientation.x,  orientation.y,  orientation.z);

				foreach (string key in keys) {
					stdout.printf("checking key %s", key);

					switch (key) {
						case "Context.Device.Orientation.displayFacingUp" :
						//	boolkey <DeviceOrientation> (orientation, key, ret,  ()=>);
						break;
						case "Context.Device.Orientation.displayFacingDown" :
						//	facing (orientation, key, ret, true);
						break;
						case "Context.Device.Orientation.inPortrait" :
							if ( orientation.rotation == "portrait" ) {
								/* TODO: clean this code when vala bug 528436 lands */
								Value truth = Value (typeof(bool));
								truth.set_boolean (true);
								TypedVariant tv = TypedVariant () {type = ValueType.TRUTH, value = truth};
								ret.insert (key, tv);
							}
						break;
						case "Context.Device.Orientation.inLandscape" :
							if ( orientation.rotation == "portrait" ) {
								/* TODO: clean this code when vala bug 528436 lands */
								Value truth = Value (typeof(bool));
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

			public Key[] Keys() {
				return keys;
			}
/*
			private void boolkey (void *data, string key, HashTable<string, TypedVariant?> ret, TrueFunction is_true, TrueFunction is_false) {
				Value truth = Value (typeof(bool));

				if (is_true(data) ) {
					truth.set_boolean (true);
				} else if ( is_false (data)) {
					truth.set_boolean (false);
				} else {
					TypedVariant tv = TypedVariant () {type = ValueType.UNDETERMINED, value = truth};
					ret.insert (key, tv);
				}
				TypedVariant tv = TypedVariant () {type = ValueType.TRUTH, value = truth};
				ret.insert (key, tv);
			}
*/
		}
	}
}
