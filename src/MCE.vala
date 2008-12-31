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
			
			// Sets of subscribers
			PluginMixins.SubscriberList orientation_subscribed;
			PluginMixins.SubscriberList display_status_subscribed;
			
			// Objects for connecting to MCE
			DBus.Connection conn;
			dynamic DBus.Object mce_request;
			dynamic DBus.Object? mce_signal;
			
			// List of keys to which someone is subscibed
			// FIXME: This approach fails to deal with unsubscriptions successfully!
			StringSet subscribed_keys = new StringSet();

			delegate void TruthFunction (void* data);
			
			// Key strings
			string key_edge_up = "Context.Device.Orientation.edgeUp";
			string key_facing_up = "Context.Device.Orientation.facingUp";
			string key_display_state = "Context.Device.Display.displayState";
			
			// Information about keys provided by this plug-in
			const Key[] keys = {
					{
						 "Context.Device.Orientation.edgeUp",
						//key_edge_up, // FIXME: Fails
						ValueType.INTEGER
						// values: Undefined (0), top (1), left (2), right (3), bottom (4)
					},
					{
						"Context.Device.Orientation.facingUp",
						//key_facing_up, 
						ValueType.INTEGER
						// Values: Undefined (0), face up (1), back side up (2)
					},
					{
						"Context.Device.Display.displayState",
						//key_display_state,
						ValueType.INTEGER
						// Values: Off (0), on (1), dimmed (2)
					}
					
				};
			
			// Keys divided into sets based on how they are got from MCE
			StringSet orientation_keys;
			StringSet display_status_keys;
			
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
			
			int calculate_orientation(int x, int y) {
				if (abs(x) > abs(y)) {
					if (x > 0) {
						// right side up
						return 3;
					}
					else {
						// left side up
						return 2;
					}
				}
				else if (abs(y) > abs(x)) {
					if (y > 0) {
						// bottom side up
						return 4;
					}
					else {
						// top side up
						return 1;
					}
				}
				// Undefined
				return 0;
			}
			
			void get_orientation (StringSet keys, HashTable<string, TypedVariant?> ret) {

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

				stdout.printf("Got orientation %s %s %s (%d,%d,%d)", orientation.rotation,  orientation.stand,  orientation.facing,  orientation.x,  orientation.y,  orientation.z);
					
				insert_orientation_to_map(keys, orientation, ret);
				
			}
			
			void get_display_status (StringSet keys, HashTable<string, TypedVariant?> ret) {

				if (keys.is_disjoint (display_status_keys)) {
					return;
			 	}

				string display_status;
				try {
					mce_request.get_display_status(out display_status);
				} catch (GLib.Error ex) {
					stdout.printf("MCE Plugin Error: %s\n", ex.message);
					error_for_subset (keys, ret, display_status_keys);
					return;
				}

				stdout.printf("Got display status %s", display_status);
					
				insert_display_status_to_map(keys, display_status, ret);
				
			}
			
			/*
			Is called when a sig_device_orientation_ind signal is received from the MCE.
			*/	
			void orientation_changed (DBus.Object sender, string rotation, string stand, string facing, int32 x, int32 y, int32 z) {
				debug ("orientation changed: %s, %s, %s", rotation, stand, facing);

				DeviceOrientation orientation = DeviceOrientation () {rotation=rotation, stand=stand, facing=facing, x=x, y=y, z=z};
				HashTable<string, TypedVariant?> ret = new HashTable<string, TypedVariant?> (str_hash,str_equal);
				
				insert_orientation_to_map(subscribed_keys, orientation, ret);
				send_result_to_listeners(orientation_subscribed, ret);
			}
			
			/*
			Inserts the orientation properties to the map according to the data given.
			*/
			void insert_orientation_to_map(StringSet keys, DeviceOrientation orientation, HashTable<string, TypedVariant?> ret) {
			if (keys.is_member (key_edge_up)) {
					Value v = Value (typeof(int));
					v.set_int(calculate_orientation(orientation.x, orientation.y));
					ret.insert (key_edge_up, TypedVariant (ValueType.INTEGER, v));
				}
				
				if (keys.is_member (key_facing_up)) {
					Value v = Value (typeof(int));
					if (orientation.facing == "face_up") {
						v.set_int(1);
					}
					else if (orientation.facing == "face_down") {
						v.set_int(2);
					}
					else
						v.set_int(0);
					
					ret.insert (key_facing_up, TypedVariant (ValueType.INTEGER, v));
				}
			}
			
			/*
			Inserts the display state properties to the map according to the data given.
			*/
			void insert_display_status_to_map(StringSet keys, string display_status, HashTable<string, TypedVariant?> ret) {
				if (keys.is_member ("Context.Device.Display.displayState")) {
					Value v = Value (typeof(int));
					
					if (display_status == "off") {
						v.set_int(0);
					}
					else if (display_status == "on") {
						v.set_int(1);
					}
					else if (display_status == "dimmed") {
						v.set_int(2);
					}
					else {
						debug ("MCE plugin: Unknown display status %s", display_status);
						return;
					}
										
					ret.insert (key_display_state, TypedVariant (ValueType.INTEGER, v));
				}
			}
			
			/*
			Is called when a display_status_ind signal is received from the MCE.
			*/
			void display_status_changed(DBus.Object sender, string display_status) {
				debug ("MCE plugin: Display status changed: %s", display_status);
				
				HashTable<string, TypedVariant?> ret = new HashTable<string, TypedVariant?> (str_hash,str_equal);
				
				insert_display_status_to_map(subscribed_keys, display_status, ret);
				send_result_to_listeners(display_status_subscribed, ret);				
			}
			
			/*
			Sends the properties to the listeners intrested in them.
			*/
			void send_result_to_listeners(PluginMixins.SubscriberList subscribers, HashTable<string, TypedVariant?> ret) {
				for (int i=0; i < subscribers.size; i++) {
					weak Subscriber s = subscribers.get(i);
					s.emit_changed(ret);
				}
			}

			void subscribe_to_orientation (StringSet keys, Subscriber s) {
				
				message ("subscribe_to_orientation: %s, %s", keys.debug(), s.object_path);

				if (keys.is_disjoint (orientation_keys)) {
					return;
			 	}

				if (orientation_subscribed.size == 0) {
					if (mce_signal == null)
						mce_signal = conn.get_object ("com.nokia.mce", "/com/nokia/mce/signal", "com.nokia.mce.signal");
					// Connect the corresponding MCE signal to its handler
					mce_signal.sig_device_orientation_ind += orientation_changed;
				}

				orientation_subscribed.add (s);
				add_keys(new StringSet.intersection (orientation_keys, keys));
			}
			
			void subscribe_to_display_status (StringSet keys, Subscriber s) {
				
				message ("subscribe_to_display_status: %s, %s", keys.debug(), s.object_path);

				if (keys.is_disjoint (display_status_keys)) {
					return;
			 	}

				if (display_status_subscribed.size == 0) {
					if (mce_signal == null)
						mce_signal = conn.get_object ("com.nokia.mce", "/com/nokia/mce/signal", "com.nokia.mce.signal");
					// Connect the corresponding MCE signal to its handler
					mce_signal.display_status_ind += display_status_changed;
				}

				display_status_subscribed.add (s);
				add_keys(new StringSet.intersection (display_status_keys, keys));
			}
			
			void add_keys(StringSet keys) {
				subscribed_keys = new StringSet.union (subscribed_keys, keys);
				// FIXME: Unsubscription.
			}
			

			void subscription_removed (PluginMixins.SubscriberList l, Subscriber s) {
				// FIXME: The subscribed_keys has to be updated in a clever way...
				// How do we know which keys are unsubscribed?
				// How do we know if someone else is anyway intrested in them?
			}

			public Plugin () {

				conn = DBus.Bus.get (DBus.BusType.SYSTEM);
				mce_request = conn.get_object ("com.nokia.mce", "/com/nokia/mce/request", "com.nokia.mce.request");

				// Data structures related to orientation
				orientation_keys = new StringSet.from_array (new string[] {
						key_edge_up, 
						key_facing_up});
				
				orientation_subscribed = new PluginMixins.SubscriberList();
				orientation_subscribed.removed += subscription_removed;
				
				// Data structures related to display status
				display_status_keys = new StringSet.from_array (new string[] {
						key_display_state});
				
				
				display_status_subscribed = new PluginMixins.SubscriberList();
				display_status_subscribed.removed += subscription_removed;
			}

			public void Get (StringSet keys, HashTable<string, TypedVariant?> ret) {
				get_orientation (keys, ret);
				get_display_status(keys, ret);
			}

			public void Subscribe (StringSet keys, ContextKit.Subscriber s) {
				subscribe_to_orientation (keys, s);
				subscribe_to_display_status (keys, s);
			}

			public Key[] Keys() {
				return keys;
			}
		}
	}
}
