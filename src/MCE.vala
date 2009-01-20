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

		public class Provider : GLib.Object, ContextKit.Provider {

			// Objects for connecting to MCE
			DBus.Connection conn;
			dynamic DBus.Object mce_request;
			dynamic DBus.Object? mce_signal;

			// Key strings
			public const string key_edge_up = "Context.Device.Orientation.edgeUp";
			public const string key_facing_up = "Context.Device.Orientation.facingUp";
			public const string key_display_state = "Context.Device.Display.displayState";
			public const string key_profile_name = "Context.Device.???.profileName"; // FIXME: key name
			// FIXME: implementation? I guess we get this from Profile Daemon, not MCE, so move this out of this plugin?
			public const string key_is_flight_mode = "Context.Environment.Connection.CurrentData.isFlightMode";
			public const string key_is_emergency_mode = "Context.Environment.Connection.CurrentData.isEmergencyMode";
			public const string key_in_active_use = "Context.Device.Usage.inActiveUse";

			// Information about keys provided by this plug-in
			public const string[] keys = {
					key_edge_up,
					key_facing_up, // Values: Undefined (0), face up (1), back side up (2)
					key_display_state, // Values: Off (0), on (1), dimmed (2)
					key_profile_name,
					key_is_flight_mode, // Values: TRUE (flight mode), FALSE (normal mode)
					key_is_emergency_mode, // Values: TRUE (emergency call in progress), FALSE (otherwise)
					key_in_active_use // Values: TRUE (device active), FALSE (device inactive)
				};

			// Keys divided into sets based on how they are got from MCE
			StringSet orientation_keys;
			StringSet display_status_keys;
			StringSet device_mode_keys;
			StringSet call_state_keys;
			StringSet inactivity_status_keys;

			void error_for_subset (StringSet keys, HashTable<string, Value?> ret, ref List<string> unavailable_keys, StringSet intersect_with) {
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
			void get_orientation (StringSet keys, HashTable<string, Value?> ret, ref List<string> unavail) {

				if (keys.is_disjoint (orientation_keys)) {
					return;
				}

				DeviceOrientation orientation = DeviceOrientation ();
				try {
					mce_request.get_device_orientation(out orientation.rotation, out orientation.stand, out orientation.facing, out orientation.x, out orientation.y, out orientation.z);
				} catch (GLib.Error ex) {
					stdout.printf("MCE Provider Error: %s\n", ex.message);
					error_for_subset (keys, ret, ref unavail, orientation_keys);
					return;
				}

				debug("Got orientation %s %s %s (%d,%d,%d)", orientation.rotation,  orientation.stand,  orientation.facing,  orientation.x,  orientation.y,  orientation.z);

				insert_orientation_to_map(keys, orientation, ret, ref unavail);

			}

			/*
			Fetches display status information from MCE.
			*/
			void get_display_status (StringSet keys, HashTable<string, Value?> ret, ref List<string> unavail) {

				if (keys.is_disjoint (display_status_keys)) {
					return;
				}

				string display_status;
				try {
					mce_request.get_display_status(out display_status);
				} catch (GLib.Error ex) {
					stdout.printf("MCE Provider Error: %s\n", ex.message);
					error_for_subset (keys, ret, ref unavail, display_status_keys);
					return;
				}

				debug("Got display status %s", display_status);

				insert_display_status_to_map(keys, display_status, ret, ref unavail);

			}

			/*
			Fetches device mode information from MCE.
			*/
			void get_device_mode(StringSet keys, HashTable<string, Value?> ret, ref List<string> unavail) {

				if (keys.is_disjoint (device_mode_keys)) {
					return;
				}

				string device_mode;
				try {
					mce_request.get_device_mode(out device_mode);
				} catch (GLib.Error ex) {
					stdout.printf("MCE Provider Error: %s\n", ex.message);
					error_for_subset (keys, ret, ref unavail, device_mode_keys);
					return;
				}

				debug("Got device mode %s", device_mode);

				insert_device_mode_to_map(keys, device_mode, ret, ref unavail);
			}

			/*
			Fetches call state information from MCE.
			*/
			void get_call_state(StringSet keys, HashTable<string, Value?> ret, ref List<string> unavail) {

				if (keys.is_disjoint (call_state_keys)) {
					return;
				}

				string state;
				string type;
				try {
					mce_request.get_call_state(out state, out type);
				} catch (GLib.Error ex) {
					stdout.printf("MCE Provider Error: %s\n", ex.message);
					error_for_subset (keys, ret, ref unavail, call_state_keys);
					return;
				}

				debug("Got call state %s %s", state, type);

				insert_call_state_to_map(keys, state, type, ret, ref unavail);
			}

			/*
			Fetches inactivity status information from MCE.
			*/
			void get_inactivity_status(StringSet keys, HashTable<string, Value?> ret, ref List<string> unavail) {

				if (keys.is_disjoint (inactivity_status_keys)) {
					return;
				}

				bool status;
				try {
					mce_request.get_inactivity_status(out status);
				} catch (GLib.Error ex) {
					stdout.printf("MCE Provider Error: %s\n", ex.message);
					error_for_subset (keys, ret, ref unavail, call_state_keys);
					return;
				}

				debug("Got inactivity status %s", (status ? "true" : "false"));

				insert_inactivity_status_to_map(keys, status, ret, ref unavail);
			}


			/*
			Inserts the orientation properties to the map according to the data given. Used by both Get and Subscribe.
			*/
			void insert_orientation_to_map(StringSet keys, DeviceOrientation orientation, HashTable<string, Value?> ret, ref List<string> unavail) {
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
			void insert_display_status_to_map(StringSet keys, string display_status, HashTable<string, Value?> ret, ref List<string> unavail) {
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
						unavail.append (key_display_state);
						return;
					}

					ret.insert (key_display_state, v);
				}
			}

			/*
			Inserts the device mode properties to the map according to the data given. Used by both Get and Subscribe.
			*/
			void insert_device_mode_to_map(StringSet keys, string device_mode, HashTable<string, Value?> ret, ref List <string> unavail) {
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
						unavail.append (key_is_flight_mode);
						return;
					}

					ret.insert (key_is_flight_mode, v);
				}
			}

			/*
			Inserts the call state properties to the map according to the data given. Used by both Get and Subscribe.
			*/
			void insert_call_state_to_map(StringSet keys, string state, string type, HashTable<string, Value?> ret, ref List<string> unavail) {
				if (keys.is_member (key_is_emergency_mode)) {
					Value v = Value (typeof(bool));

					if (state == "none") {
						v.set_boolean(false);
					}
					else if (state == "cellular" || state == "voip" || state == "video") {
						if (type == "normal") {
							v.set_boolean(false);
						}
						else if (type == "emergency") {
							v.set_boolean(true);
						}
						else {
							stdout.printf ("MCE plugin: Unknown call type %s\n", type);
							unavail.append (key_is_emergency_mode);
							return;
						}
					}
					else {
						stdout.printf ("MCE plugin: Unknown call state %s\n", state);
						unavail.append (key_is_emergency_mode);
						return;
					}

					ret.insert (key_is_emergency_mode, v);
				}
			}

			/*
			Inserts the inactivity status properties to the map according to the data given. Used by both Get and Subscribe.
			*/
			void insert_inactivity_status_to_map(StringSet keys, bool status, HashTable<string, Value?> ret, ref List<string> unavail) {
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

				insert_orientation_to_map(orientation_keys, orientation, ret, ref unavail);

				// Update the central value table with the new property values
				Main.the_manager.property_values_changed(ret, unavail);
				// FIXME: Is this how it should be done? 
			}

			/*
			Is called when a display_status_ind signal is received from MCE.
			*/
			void display_status_changed(DBus.Object sender, string display_status) {
				debug ("MCE plugin: Display status changed: %s", display_status);

				HashTable<string, Value?> ret = new HashTable<string, Value?> (str_hash,str_equal);
				List<string> unavail = new List<string>();

				insert_display_status_to_map(display_status_keys, display_status, ret, ref unavail);

				// Update the central value table with the new property values
				// FIXME
			}

			/*
			Is called when a sig_device_mode_ind signal is received from MCE.
			*/
			void device_mode_changed(DBus.Object sender, string device_mode) {
				debug ("MCE plugin: Device mode changed: %s", device_mode);

				HashTable<string, Value?> ret = new HashTable<string, Value?> (str_hash,str_equal);
				List<string> unavail = new List<string>();

				insert_device_mode_to_map(device_mode_keys, device_mode, ret, ref unavail);

				// Update the central value table with the new property values
				// FIXME
			}

			/*
			Is called when a sig_call_state_ind signal is received from MCE.
			*/
			void call_state_changed(DBus.Object sender, string state, string type) {
				debug ("MCE plugin: Call state changed: %s %s", state, type);

				HashTable<string, Value?> ret = new HashTable<string, Value?> (str_hash,str_equal);
				List<string> unavail = new List<string>();

				insert_call_state_to_map(call_state_keys, state, type, ret, ref unavail);

				// Update the central value table with the new property values
				// FIXME
			}

			/*
			Is called when a system_call_state_ind signal is received from MCE.
			*/
			void inactivity_status_changed(DBus.Object sender, bool status) {
				debug ("MCE plugin: Inactivity state changed: %s", (status ? "true" : "false"));

				HashTable<string, Value?> ret = new HashTable<string, Value?> (str_hash,str_equal);
				List<string> unavail = new List<string>();

				insert_inactivity_status_to_map(inactivity_status_keys, status, ret, ref unavail);

				// Update the central value table with the new property values
				// FIXME
			}

			public Provider () {

				conn = DBus.Bus.get (DBus.BusType.SYSTEM);
				mce_request = conn.get_object ("com.nokia.mce", "/com/nokia/mce/request", "com.nokia.mce.request");

				// Data structures related to orientation
				orientation_keys = new StringSet.from_array (new string[] {
						key_edge_up,
						key_facing_up});

				// Data structures related to display status
				display_status_keys = new StringSet.from_array (new string[] {
						key_display_state});

				// Data structures related to device mode (normal / flight)
				device_mode_keys = new StringSet.from_array (new string[] {
						key_is_flight_mode});

				// Data structures related to call state (emergency / no emergency)
				call_state_keys = new StringSet.from_array (new string[] {
						key_is_emergency_mode});

				// Data structures related to inactivity status
				inactivity_status_keys = new StringSet.from_array (new string[] {
						key_in_active_use});

			}

			public void Get (StringSet keys, HashTable<string, Value?> ret, ref List<string> unavail) {
				get_orientation (keys, ret, ref unavail);
				get_display_status (keys, ret, ref unavail);
				get_device_mode (keys, ret, ref unavail);
				get_call_state (keys, ret, ref unavail);
				get_inactivity_status (keys, ret, ref unavail);
			}

			private void ensure_mce_signal_exists() {
				if (mce_signal == null) {
					mce_signal = conn.get_object ("com.nokia.mce", "/com/nokia/mce/signal", "com.nokia.mce.signal");
				}
			}

			/* FIXME: replace Subscribe with below with: */

			public void KeysSubscribed (StringSet keys) {

				debug ("MCE plugin: KeysSubscribed %s", keys.debug());
				ensure_mce_signal_exists();

				// Connect the corresponding MCE signal to its handler
				if (keys.is_disjoint (orientation_keys) == false) {
					// Note: even thoug orientation_keys has to members,
					// this is OK since connecting the same signal twice does not do any harm

					mce_signal.sig_device_orientation_ind += orientation_changed;
				}

				if (keys.is_disjoint (display_status_keys) == false) {
					mce_signal.display_status_ind += display_status_changed;
				}

				if (keys.is_disjoint (device_mode_keys) == false) {
					mce_signal.sig_device_mode_ind += device_mode_changed;
				}

				if (keys.is_disjoint (call_state_keys) == false) {
					mce_signal.sig_call_state_ind += call_state_changed;
				}

				if (keys.is_disjoint (inactivity_status_keys) == false) {
					mce_signal.system_inactivity_ind += inactivity_status_changed;
				}
			}

			public void KeysUnsubscribed (StringSet keys) {
				
				debug ("MCE plugin: KeysUnsubscribed %s", keys.debug());
				// Disconnect the corresponding MCE signal from its handler
				if (keys.is_disjoint (orientation_keys) == false) {
					// Note: orientation_keys contains two keys
					// At least one of the keys was unsubscribed, but the other one may still be subscribed to
					// FIXME: How to implement?
					int no_of_subscribers = 0;
					foreach (var key in orientation_keys) {
						//no_of_subscribers += Main.the_manager.key_counter.number_of_subscribers(key);
					}
					
					if (no_of_subscribers == 0) {
						mce_signal.sig_device_orientation_ind -= orientation_changed;
					}
				}

				// The other key sets contain only one key, so unsubscription is straightforward
				if (keys.is_disjoint (display_status_keys) == false) {
					mce_signal.display_status_ind -= display_status_changed;
				}

				if (keys.is_disjoint (device_mode_keys) == false) {
					mce_signal.sig_device_mode_ind -= device_mode_changed;
				}

				if (keys.is_disjoint (call_state_keys) == false) {
					mce_signal.sig_call_state_ind -= call_state_changed;
				}

				if (keys.is_disjoint (inactivity_status_keys) == false) {
					mce_signal.system_inactivity_ind -= inactivity_status_changed;
				}
			}
		}
	}
}
