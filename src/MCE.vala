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
			PluginMixins.SubscriberList orientation_subscribed;
			DBus.Connection conn;
			dynamic DBus.Object mce_request;
			dynamic DBus.Object? mce_signal;
			StringSet subscribed_keys = new StringSet();

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

			void check_orientation_get (StringSet keys, HashTable<string, TypedVariant?> ret) {

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

			void orientation_changed (DBus.Object sender, string rotation, string stand, string facing, int32 x, int32 y, int32 z) {
				message ("orientation changed: %s, %s, %s", rotation, stand, facing);

				DeviceOrientation orientation = DeviceOrientation () {rotation=rotation, stand=stand, facing=facing, x=x, y=y, z=z};
				HashTable<string, TypedVariant?> ret = new HashTable<string, TypedVariant?> (str_hash,str_equal);
				

				if (subscribed_keys.is_member ("Context.Device.Orientation.displayFacingUp")) {
					boolkey ("Context.Device.Orientation.displayFacingUp", ret, orientation.facing, "face_up", "face_down");
				}

				if (subscribed_keys.is_member ("Context.Device.Orientation.displayFacingDown")) {
					boolkey ("Context.Device.Orientation.displayFacingDown", ret, orientation.facing, "face_down", "face_up");
				}

				if (subscribed_keys.is_member ("Context.Device.Orientation.inPortrait")) {
					boolkey ("Context.Device.Orientation.inPortrait", ret, orientation.rotation, "portrait", "landscape");
				}

				if (subscribed_keys.is_member ("Context.Device.Orientation.inLandscape")) {
					boolkey ("Context.Device.Orientation.inLandscape", ret, orientation.rotation, "landscape", "portrait");
				}

				for (int i=0; i < orientation_subscribed.size; i++) {
					weak Subscriber s =orientation_subscribed.get(i);
					s.emit_changed(ret);
				}
			}

			void check_orientation_subscribe (StringSet keys, Subscriber s) {
				
				message ("check_orientation_subscribe: %s, %s", keys.debug(), s.object_path);

				if (keys.is_disjoint (orientation_keys)) {
					return;
			 	}

				if (orientation_subscribed.size == 0) {
					if (mce_signal == null)
						mce_signal = conn.get_object ("com.nokia.mce", "/com/nokia/mce/signal", "com.nokia.mce.signal");
					mce_signal.sig_device_orientation_ind += orientation_changed;
				}

				orientation_subscribed.add (s);
				subscribed_keys = new StringSet.union (subscribed_keys, keys);
			}

			void subscription_removed (PluginMixins.SubscriberList l, Subscriber s) {
			}

			public Plugin () {

				conn = DBus.Bus.get (DBus.BusType.SYSTEM);
				mce_request = conn.get_object ("com.nokia.mce", "/com/nokia/mce/request", "com.nokia.mce.request");
				orientation_keys = new StringSet.from_array (new string[] {
						"Context.Device.Orientation.displayFacingUp",
						"Context.Device.Orientation.displayFacingDown",
						"Context.Device.Orientation.inLandscape",
						"Context.Device.Orientation.inPortrait"});

				orientation_subscribed = new PluginMixins.SubscriberList();
				orientation_subscribed.removed += subscription_removed;
			}


			public void Get (StringSet keys, HashTable<string, TypedVariant?> ret) {
				check_orientation_get (keys, ret);
			}

			public void Subscribe (StringSet keys, ContextKit.Subscriber s) {
				check_orientation_subscribe (keys, s);
			}

			public Key[] Keys() {
				return keys;
			}
		}
	}
}
