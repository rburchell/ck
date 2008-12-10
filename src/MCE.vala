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
						"Context.Device.orientation",
						ValueType.INTEGER
					}
				};

			StringSet orientation_keys;
			
			void error_for_subset (StringSet keys, HashTable<string, TypedVariant?> ret, StringSet intersect_with) {
				StringSet intersection = new StringSet.intersection (keys, intersect_with);
				foreach (var key in intersection.to_array()) {
					Value nonsense = Value (typeof (bool));
					nonsense.set_boolean (false);
					ret.insert (key, TypedVariant (ValueType.UNDETERMINED, nonsense));
				}

			}
			
			int abs(int n) {
				if (n > 0) {
					return n;
				}
				return -n;
			}
			
			int calculate_orientation(int x, int y, int z) {
				if (abs(x) > abs(y) && abs(x) > abs(z)) {
					if (x > 0) {
						// right side up
						return 3;
					}
					else {
						// left side up
						return 2;
					}
				}
				else if (abs(y) > abs(x) && abs(y) > abs(z)) {
					if (y > 0) {
						// bottom side up
						return 4;
					}
					else {
						// top side up
						return 1;
					}
				}
				else if (abs(z) > abs(x) && abs(z) > abs(y)) {
					if (z > 0) {
						// back side up
						return 6;
					}
					else {
						// front side up
						return 5;
					}
				}
				// Undefined
				return 0;
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
					
				if (keys.is_member ("Context.Device.orientation")) {
					Value v = Value (typeof(int));
					v.set_int(calculate_orientation(orientation.x, orientation.y, orientation.z));
					ret.insert ("Context.Device.orientation", TypedVariant (ValueType.INTEGER, v));
				}
				
			}

			void orientation_changed (DBus.Object sender, string rotation, string stand, string facing, int32 x, int32 y, int32 z) {
				message ("orientation changed: %s, %s, %s", rotation, stand, facing);

				DeviceOrientation orientation = DeviceOrientation () {rotation=rotation, stand=stand, facing=facing, x=x, y=y, z=z};
				HashTable<string, TypedVariant?> ret = new HashTable<string, TypedVariant?> (str_hash,str_equal);
				
				if (subscribed_keys.is_member ("Context.Device.orientation")) {
					Value v = Value (typeof(int));
					v.set_int(calculate_orientation(orientation.x, orientation.y, orientation.z));
					ret.insert ("Context.Device.orientation", TypedVariant (ValueType.INTEGER, v));
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
						"Context.Device.orientation"});

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
