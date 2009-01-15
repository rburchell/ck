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
			PluginMixins.SubscriberList device_mode_subscribed;
			PluginMixins.SubscriberList call_state_subscribed;
			PluginMixins.SubscriberList inactivity_status_subscribed;

			// Objects for connecting to MCE
			DBus.Connection conn;
			dynamic DBus.Object mce_request;
			dynamic DBus.Object? mce_signal;

			// List of keys to which someone is subscibed
			// FIXME: This approach fails to deal with unsubscriptions successfully!
			StringSet subscribed_keys = new StringSet();

			delegate void TruthFunction (void* data);

			// Key strings
			const string key_edge_up = "Context.Device.Orientation.edgeUp";
			const string key_facing_up = "Context.Device.Orientation.facingUp";
			const string key_display_state = "Context.Device.Display.displayState";
			const string key_profile_name = "Context.Device.???.profileName"; // FIXME: key name
			// FIXME: implementation? I guess we get this from Profile Daemon, not MCE, so move this out of this plugin?
			const string key_is_flight_mode = "Context.Environment.Connection.CurrentData.isFlightMode";
			const string key_is_emergency_mode = "Context.Environment.Connection.CurrentData.isEmergencyMode";
			const string key_in_active_use = "Context.Device.Usage.inActiveUse";

			// Information about keys provided by this plug-in
			const Key[] keys = {
					{
						key_edge_up
						// Values: Undefined (0), top (1), left (2), right (3), bottom (4)
					},
					{
						key_facing_up
						// Values: Undefined (0), face up (1), back side up (2)
					},
					{
						key_display_state
						// Values: Off (0), on (1), dimmed (2)
					},
					{
						key_profile_name
						// Values:
					},
					{
						key_is_flight_mode
						// Values: TRUE (flight mode), FALSE (normal mode)
					},
					{
						key_is_emergency_mode
						// Values: TRUE (emergency call in progress), FALSE (otherwise)
					},
					{
						key_in_active_use
						// Values: TRUE (device active), FALSE (device inactive)
					}
				};

			// Keys divided into sets based on how they are got from MCE
			StringSet orientation_keys;
			StringSet display_status_keys;
			StringSet device_mode_keys;
			StringSet call_state_keys;
			StringSet inactivity_status_keys;

			void error_for_subset (StringSet keys, HashTable<string, Value?> ret, List<string> unavailable_keys, StringSet intersect_with) {
				StringSet intersection = new StringSet.intersection (keys, intersect_with);
				foreach (var key in intersection.to_array()) {
					Value nonsense = Value (typeof (bool));
					nonsense.set_boolean (false);
					unavailable_keys.prepend (key);
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
						// Right side up
						return 3;
					}
					else {
						// Left side up
						return 2;
					}
				}
				else if (abs(y) > abs(x)) {
					if (y > 0) {
						// Bottom side up
						return 4;
					}
					else {
						// Top side up
						return 1;
					}
				}
				// Undefined
				return 0;
			}

			/*
			Fetches orientation information from MCE.
			*/
			void get_orientation (StringSet keys, HashTable<string, Value?> ret, List<string> unavail) {

				if (keys.is_disjoint (orientation_keys)) {
					return;
				}

				DeviceOrientation orientation = DeviceOrientation ();
				try {
					mce_request.get_device_orientation(out orientation.rotation, out orientation.stand, out orientation.facing, out orientation.x, out orientation.y, out orientation.z);
				} catch (GLib.Error ex) {
					stdout.printf("MCE Plugin Error: %s\n", ex.message);
					error_for_subset (keys, ret, unavail, orientation_keys);
					return;
				}

				debug("Got orientation %s %s %s (%d,%d,%d)", orientation.rotation,  orientation.stand,  orientation.facing,  orientation.x,  orientation.y,  orientation.z);

				insert_orientation_to_map(keys, orientation, ret, unavail);

			}

			/*
			Fetches display status information from MCE.
			*/
			void get_display_status (StringSet keys, HashTable<string, Value?> ret, List<string> unavail) {

				if (keys.is_disjoint (display_status_keys)) {
					return;
				}

				string display_status; // FIXME: Check: Does Vala handle memory management correctly?
				try {
					mce_request.get_display_status(out display_status);
				} catch (GLib.Error ex) {
					stdout.printf("MCE Plugin Error: %s\n", ex.message);
					error_for_subset (keys, ret, unavail, display_status_keys);
					return;
				}

				debug("Got display status %s", display_status);

				insert_display_status_to_map(keys, display_status, ret, unavail);

			}

			/*
			Fetches device mode information from MCE.
			*/
			void get_device_mode(StringSet keys, HashTable<string, Value?> ret, List<string> unavail) {

				if (keys.is_disjoint (device_mode_keys)) {
					return;
				}

				string device_mode; // FIXME: Check: Does Vala handle memory management correctly?
				try {
					mce_request.get_device_mode(out device_mode);
				} catch (GLib.Error ex) {
					stdout.printf("MCE Plugin Error: %s\n", ex.message);
					error_for_subset (keys, ret, unavail, device_mode_keys);
					return;
				}

				debug("Got device mode %s", device_mode);

				insert_device_mode_to_map(keys, device_mode, ret, unavail);
			}

			/*
			Fetches call state information from MCE.
			*/
			void get_call_state(StringSet keys, HashTable<string, Value?> ret, List<string> unavail) {

				if (keys.is_disjoint (call_state_keys)) {
					return;
				}

				string state; // FIXME: Check: Does Vala handle memory management correctly?
				string type;
				try {
					mce_request.get_call_state(out state, out type);
				} catch (GLib.Error ex) {
					stdout.printf("MCE Plugin Error: %s\n", ex.message);
					error_for_subset (keys, ret, unavail, call_state_keys);
					return;
				}

				debug("Got call state %s %s", state, type);

				insert_call_state_to_map(keys, state, type, ret, unavail);
			}

			/*
			Fetches inactivity status information from MCE.
			*/
			void get_inactivity_status(StringSet keys, HashTable<string, Value?> ret, List<string> unavail) {

				if (keys.is_disjoint (inactivity_status_keys)) {
					return;
				}

				bool status;
				try {
					mce_request.get_inactivity_status(out status);
				} catch (GLib.Error ex) {
					stdout.printf("MCE Plugin Error: %s\n", ex.message);
					error_for_subset (keys, ret, unavail, call_state_keys);
					return;
				}

				debug("Got inactivity status %s", (status ? "true" : "false"));

				insert_inactivity_status_to_map(keys, status, ret, unavail);
			}


			/*
			Inserts the orientation properties to the map according to the data given. Used by both Get and Subscribe.
			*/
			void insert_orientation_to_map(StringSet keys, DeviceOrientation orientation, HashTable<string, Value?> ret, List<string> unavail) {
				if (keys.is_member (key_edge_up)) {
					Value v = Value (typeof(int));
					v.set_int(calculate_orientation(orientation.x, orientation.y));
					ret.insert (key_edge_up, v);
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

					ret.insert (key_facing_up, v);
				}
			}

			/*
			Inserts the display state properties to the map according to the data given. Used by both Get and Subscribe.
			*/
			void insert_display_status_to_map(StringSet keys, string display_status, HashTable<string, Value?> ret, List<string> unavail) {
				if (keys.is_member (key_display_state)) {
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
						stdout.printf ("MCE plugin: Unknown display status %s\n", display_status);
						return;
					}

					ret.insert (key_display_state, v);
				}
			}

			/*
			Inserts the device mode properties to the map according to the data given. Used by both Get and Subscribe.
			*/
			void insert_device_mode_to_map(StringSet keys, string device_mode, HashTable<string, Value?> ret, List <string> unavail) {
				if (keys.is_member (key_is_flight_mode)) {
					Value v = Value (typeof(bool));

					if (device_mode == "normal") {
						v.set_boolean(false);
					}
					else if (device_mode == "flight") {
						v.set_boolean(true);
					}
					else {
						stdout.printf ("MCE plugin: Unknown device mode %s\n", device_mode);
						return;
					}

					ret.insert (key_is_flight_mode, v);
				}
			}

			/*
			Inserts the call state properties to the map according to the data given. Used by both Get and Subscribe.
			*/
			void insert_call_state_to_map(StringSet keys, string state, string type, HashTable<string, Value?> ret, List<string> unavail) {
				if (keys.is_member (key_is_emergency_mode)) {
					Value v = Value (typeof(bool));

					if (state == "none") {
						v.set_boolean(false);
					}
					else if (state == "cellular" || state == "voip" || state == "video" || state == "none") {
						if (type == "normal") {
							v.set_boolean(false);
						}
						else if (type == "emergency") {
							v.set_boolean(true);
						}
						else {
							stdout.printf ("MCE plugin: Unknown call type %s\n", type);
							return;
						}
					}
					else {
						stdout.printf ("MCE plugin: Unknown call state %s\n", state);
						return;
					}

					ret.insert (key_is_emergency_mode, v);
				}
			}

			/*
			Inserts the inactivity status properties to the map according to the data given. Used by both Get and Subscribe.
			*/
			void insert_inactivity_status_to_map(StringSet keys, bool status, HashTable<string, Value?> ret, List<string> unavail) {
				if (keys.is_member (key_in_active_use)) {
					Value v = Value (typeof(bool));
					v.set_boolean(!status);	// Note the negation

					ret.insert (key_in_active_use, v);
				}
			}

			/*
			Is called when a sig_device_orientation_ind signal is received from MCE.
			*/
			void orientation_changed (DBus.Object sender, string rotation, string stand, string facing, int32 x, int32 y, int32 z) {
				debug("orientation_changed: %s, %s, %s", rotation, stand, facing);

				DeviceOrientation orientation = DeviceOrientation () {rotation=rotation, stand=stand, facing=facing, x=x, y=y, z=z};
				HashTable<string, Value?> ret = new HashTable<string, Value?> (str_hash,str_equal);
				List<string> unavail = new List<string>();

				insert_orientation_to_map(subscribed_keys, orientation, ret, unavail);
				send_result_to_listeners(orientation_subscribed, ret, unavail);
			}

			/*
			Is called when a display_status_ind signal is received from MCE.
			*/
			void display_status_changed(DBus.Object sender, string display_status) {
				debug ("MCE plugin: Display status changed: %s", display_status);

				HashTable<string, Value?> ret = new HashTable<string, Value?> (str_hash,str_equal);
				List<string> unavail = new List<string>();

				insert_display_status_to_map(subscribed_keys, display_status, ret, unavail);
				send_result_to_listeners(display_status_subscribed, ret, unavail);
			}

			/*
			Is called when a sig_device_mode_ind signal is received from MCE.
			*/
			void device_mode_changed(DBus.Object sender, string device_mode) {
				debug ("MCE plugin: Device mode changed: %s", device_mode);

				HashTable<string, Value?> ret = new HashTable<string, Value?> (str_hash,str_equal);
				List<string> unavail = new List<string>();

				insert_device_mode_to_map(subscribed_keys, device_mode, ret, unavail);
				send_result_to_listeners(device_mode_subscribed, ret, unavail);
			}

			/*
			Is called when a sig_call_state_ind signal is received from MCE.
			*/
			void call_state_changed(DBus.Object sender, string state, string type) {
				debug ("MCE plugin: Call state changed: %s %s", state, type);

				HashTable<string, Value?> ret = new HashTable<string, Value?> (str_hash,str_equal);
				List<string> unavail = new List<string>();

				insert_call_state_to_map(subscribed_keys, state, type, ret, unavail);
				send_result_to_listeners(call_state_subscribed, ret, unavail);
			}

			/*
			Is called when a system_call_state_ind signal is received from MCE.
			*/
			void inactivity_status_changed(DBus.Object sender, bool status) {
				debug ("MCE plugin: Inactivity state changed: %s", (status ? "true" : "false"));

				HashTable<string, Value?> ret = new HashTable<string, Value?> (str_hash,str_equal);
				List<string> unavail = new List<string>();

				insert_inactivity_status_to_map(subscribed_keys, status, ret, unavail);
				send_result_to_listeners(inactivity_status_subscribed, ret, unavail);
			}

			/*
			Sends the properties to the listeners intrested in them.
			*/
			void send_result_to_listeners(PluginMixins.SubscriberList subscribers, HashTable<string, Value?> ret, List<string> unavail) {
				for (int i=0; i < subscribers.size; i++) {
					weak Subscriber s = subscribers.get(i);
					s.emit_changed(ret, unavail);
				}
			}

			void subscribe_to_orientation (StringSet keys, Subscriber s) {

				debug ("subscribe_to_orientation: %s, %s", keys.debug(), s.object_path);

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

				debug ("subscribe_to_display_status: %s, %s", keys.debug(), s.object_path);

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

			void subscribe_to_device_mode (StringSet keys, Subscriber s) {

				debug ("subscribe_to_device_mode: %s, %s", keys.debug(), s.object_path);

				if (keys.is_disjoint (device_mode_keys)) {
					return;
				}

				if (device_mode_subscribed.size == 0) {
					if (mce_signal == null)
						mce_signal = conn.get_object ("com.nokia.mce", "/com/nokia/mce/signal", "com.nokia.mce.signal");
					// Connect the corresponding MCE signal to its handler
					mce_signal.sig_device_mode_ind += device_mode_changed;
				}

				device_mode_subscribed.add (s);
				add_keys(new StringSet.intersection (device_mode_keys, keys));
			}

			void subscribe_to_call_state (StringSet keys, Subscriber s) {

				debug ("subscribe_to_call_state: %s, %s", keys.debug(), s.object_path);

				if (keys.is_disjoint (call_state_keys)) {
					return;
				}

				if (call_state_subscribed.size == 0) {
					if (mce_signal == null)
						mce_signal = conn.get_object ("com.nokia.mce", "/com/nokia/mce/signal", "com.nokia.mce.signal");
					// Connect the corresponding MCE signal to its handler
					mce_signal.sig_call_state_ind += call_state_changed;
				}

				call_state_subscribed.add (s);
				add_keys(new StringSet.intersection (call_state_keys, keys));
			}

			void subscribe_to_inactivity_status (StringSet keys, Subscriber s) {

				debug ("subscribe_to_inactivity_status: %s, %s", keys.debug(), s.object_path);

				if (keys.is_disjoint (inactivity_status_keys)) {
					return;
				}

				if (inactivity_status_subscribed.size == 0) {
					if (mce_signal == null)
						mce_signal = conn.get_object ("com.nokia.mce", "/com/nokia/mce/signal", "com.nokia.mce.signal");
					// Connect the corresponding MCE signal to its handler
					mce_signal.system_inactivity_ind += inactivity_status_changed;
				}

				inactivity_status_subscribed.add (s);
				add_keys(new StringSet.intersection (inactivity_status_keys, keys));
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

				orientation_subscribed.removed += subscription_removed;

				// Data structures related to display status
				display_status_keys = new StringSet.from_array (new string[] {
						key_display_state});

				display_status_subscribed = new PluginMixins.SubscriberList();
				display_status_subscribed.removed += subscription_removed;

				// Data structures related to device mode (normal / flight)
				device_mode_keys = new StringSet.from_array (new string[] {
						key_is_flight_mode});

				device_mode_subscribed = new PluginMixins.SubscriberList();
				device_mode_subscribed.removed += subscription_removed;

				// Data structures related to call state (emergency / no emergency)
				call_state_keys = new StringSet.from_array (new string[] {
						key_is_emergency_mode});

				call_state_subscribed = new PluginMixins.SubscriberList();
				call_state_subscribed.removed += subscription_removed;

				// Data structures related to inactivity status
				inactivity_status_keys = new StringSet.from_array (new string[] {
						key_in_active_use});

				inactivity_status_subscribed = new PluginMixins.SubscriberList();
				inactivity_status_subscribed.removed += subscription_removed;
			}

			public void Get (StringSet keys, HashTable<string, Value?> ret, List<string> unavail) {
				get_orientation (keys, ret, unavail);
				get_display_status (keys, ret, unavail);
				get_device_mode (keys, ret, unavail);
				get_call_state (keys, ret, unavail);
				get_inactivity_status (keys, ret, unavail);
			}

			public void Subscribe (StringSet keys, ContextKit.Subscriber s) {
				subscribe_to_orientation (keys, s);
				subscribe_to_display_status (keys, s);
				subscribe_to_device_mode (keys, s);
				subscribe_to_call_state (keys, s);
				subscribe_to_inactivity_status (keys, s);
			}

			public Key[] Keys() {
				return keys;
			}
		}
	}
}
