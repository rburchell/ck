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

		public class Plugin : GLib.Object, ContextKit.Plugin {
			PluginMixins.SubscriptionList <Plugin> list;
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

			StringSet orientation_keys;
			
			void boolkey (string key, HashTable<string, TypedVariant?> ret, string input, string true_value, string false_value) {
				Value truth = Value (typeof(bool));

				if (input == true_value) {
					truth.set_boolean (true);
				} else if ( input == false_value) {
					truth.set_boolean (false);
				} else {
					truth.set_boolean (false);
					ret.insert (key, TypedVariant (ValueType.UNDETERMINED, truth));
					return;
				}
				ret.insert (key, TypedVariant (ValueType.TRUTH, truth));
			}

			void error_for_subset (StringSet keys, HashTable<string, TypedVariant?> ret, StringSet intersect_with) {
				StringSet intersection = new StringSet.intersection (keys, intersect_with);
				foreach (var key in intersection.to_array()) {
					Value nonsense = Value (typeof (bool));
					nonsense.set_boolean (false);
					ret.insert (key, TypedVariant (ValueType.UNDETERMINED, nonsense));
				}

			}

			void check_orientation (StringSet keys, HashTable<string, TypedVariant?> ret) {

				if (keys.is_disjoint (orientation_keys)) {
					return;
			 	}

				DeviceOrientation orientation = DeviceOrientation ();
				try {
					mce_request.get_device_orientation(out orientation.rotation, out orientation.stand, out orientation.facing, out orientation.x, out orientation.y, out orientation.z);
				} catch (GLib.Error ex) {
					stdout.printf("MCE Plugin Error: %s\n", ex.message);
					error_for_subset (keys, ret, orientation_keys);
					return;
				}

				stdout.printf("got orientation %s %s %s (%d,%d,%d)", orientation.rotation,  orientation.stand,  orientation.facing,  orientation.x,  orientation.y,  orientation.z);

				if (keys.is_member ("Context.Device.Orientation.displayFacingUp")) {
					boolkey ("Context.Device.Orientation.displayFacingUp", ret, orientation.facing, "face_up", "face_down");
				}

				if (keys.is_member ("Context.Device.Orientation.displayFacingDown")) {
					boolkey ("Context.Device.Orientation.displayFacingDown", ret, orientation.facing, "face_down", "face_up");
				}

				if (keys.is_member ("Context.Device.Orientation.inPortrait")) {
					boolkey ("Context.Device.Orientation.inPortrait", ret, orientation.rotation, "portrait", "landscape");
				}

				if (keys.is_member ("Context.Device.Orientation.inLandscape")) {
					boolkey ("Context.Device.Orientation.inLandscape", ret, orientation.rotation, "landscape", "portrait");
				}

			}

			void subscription_removed (Subscription s) {
			}

			public Plugin () {
				list = new PluginMixins.SubscriptionList <Plugin> (this, subscription_removed);

				conn = DBus.Bus.get (DBus.BusType.SYSTEM);
				mce_request = conn.get_object ("com.nokia.mce", "/com/nokia/mce/request", "com.nokia.mce.request");
				mce_signal = conn.get_object ("com.nokia.mce", "/com/nokia/mce/signal", "com.nokia.mce.signal");
				orientation_keys = new StringSet.from_array (new string[] {
						"Context.Device.Orientation.displayFacingUp",
						"Context.Device.Orientation.displayFacingDown",
						"Context.Device.Orientation.inLandscape",
						"Context.Device.Orientation.inPortrait"});
			}


			public void Get (StringSet keys, HashTable<string, TypedVariant?> ret) {
				check_orientation (keys, ret);
			}

			public void Subscribe (StringSet keys, ContextKit.Subscription s) {
			}

			public Key[] Keys() {
				return keys;
			}
		}
	}
}
