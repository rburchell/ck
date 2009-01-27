/*
FIXME: Remember to unref the needed things!

*/


namespace ContextKit {

	namespace Location2 {
		// Data type for the locally stored location data
		struct LocationData {
			public double latitude;
			public double longitude;
			public double altitude;

			public bool latlong_valid;
			public bool altitude_valid;
		}

		public class Provider : GLib.Object, ContextKit.Provider {
			ProviderMixins.SubscriberList subscribed;

			// contextd keys this plugin takes care of
			const Key[] keys = {
				{
					"Context.Environment.Location.latitude",
					ValueType.DOUBLE
				},
				{
					"Context.Environment.Location.longitude",
					ValueType.DOUBLE
				},
				{
					"Context.Environment.Location.altitude",
					ValueType.DOUBLE
				}
			};

			StringSet location_keys;

			StringSet subscribed_keys = new StringSet();

			// Stores the last known location
			static LocationData current_location;

			// Device for using liblocation
			Location.GPSDevice m_device;

			void subscription_removed (ProviderMixins.SubscriberList l, Subscriber s) {
				// TODO: Check here whether the connection to the loc service should be closed? 

				// FIXME: Do we have to remove it from the subscription list manually? Should the subscription list be part of the superclass?
			}

			/* Constructor
			Connect the liblocation signals to correct functions.
			Possibly: Indicate to liblocation that we're intrested in location data and start listening.
			*/
			public Provider () {

				// Create plugin-related data structures
				subscribed = new ProviderMixins.SubscriberList();
				subscribed.removed += subscription_removed;

				location_keys = new StringSet.from_array (new string[] {
						"Context.Environment.Location.latitude",
						"Context.Environment.Location.longitude",
						"Context.Environment.Location.altitude"
						});

				// Invalidate the initial data
				current_location.latlong_valid = false;
				current_location.altitude_valid = false;

				// Fetch an instance of the LocationGPSDControl
				Location.GPSDControl control;
				control = control.get_default();

				// Connect signals from LocationGPSDControl
				control.error += CB_Error;
				control.gpsd_running += CB_Running;
				control.gpsd_stopped += CB_Stopped;

				// Indicate that we want to use only ACWP to avoid starting the GPS if it is not already running.
				control.preferred_method = Location.Method.ACWP;

				control.start(); // FIXME: This is only for testing.

				// Create a LocationGPSDevice
				m_device = new Location.GPSDevice();

				// Connect signals from device
				m_device.changed += CB_Changed;
				m_device.connected += CB_Connected;
				m_device.disconnected += CB_Disconnected;

				m_device.start(); // FIXME: This is only for testing

				// FIXME: Do we need to unref control? Or does Vala handle it automatically?

			}

			// Destructor
			~Provider() {
				// FIXME: Deleting / freeing / ... the device? How? Or does Vala handle it automatically?
			}

			// Handling signals from GPS control
			static void CB_Error(Location.GPSDControl? control) {
				debug("Error");
			}

			static void CB_Running(Location.GPSDControl? control) {
				debug("Running");
			}

			static void CB_Stopped(Location.GPSDControl? control) {
				debug("Stopped");
			}

			// Handling signals from GPS device
			void CB_Changed(Location.GPSDevice? device) {

				bool values_changed = false;
				if (device != null && device.fix != null) { 
					debug("Changed %f %f %f", device.fix.latitude, device.fix.longitude, device.fix.altitude);
					if ((device.fix.fields & Location.GPS_DEVICE_LATLONG_SET) != 0) {

						if (current_location.latitude != device.fix.latitude) {
							values_changed = true;
						}
						if (current_location.longitude != device.fix.longitude) {
							values_changed = true;
						}
						current_location.latitude = device.fix.latitude;
						current_location.longitude = device.fix.longitude;
						current_location.latlong_valid = true;
					}

					if ((device.fix.fields & Location.GPS_DEVICE_ALTITUDE_SET) != 0) {
						if (current_location.altitude != device.fix.altitude) {
							values_changed = true;
						}

						current_location.altitude = device.fix.altitude;
						current_location.altitude_valid = true;
					}
					// TODO: Tresholds? If the values change only a little, ignore the change?
				}

				HashTable<string, TypedVariant?> ret = new HashTable<string, TypedVariant?> (str_hash,str_equal);

				if (subscribed_keys.is_member ("Context.Environment.Location.latitude")) {
					insert_double_key("Context.Environment.Location.latitude", ret, current_location.latitude, current_location.latlong_valid);
				}

				if (subscribed_keys.is_member ("Context.Environment.Location.longitude")) {
					insert_double_key("Context.Environment.Location.longitude", ret, current_location.longitude, current_location.latlong_valid);
				}

				if (subscribed_keys.is_member ("Context.Environment.Location.altitude")) {
					insert_double_key("Context.Environment.Location.altitude", ret, current_location.altitude, current_location.altitude_valid);
				}

				// TODO: Should we check whether we need the location service any more?
				// If we stop the connecton, we won't be able to answer future Get:s accurately.

				/* Note: We always emit the same kind of changed signal, not depending on which values have
				actually changed. */

				if (values_changed) {
					// Emit the "Changed" signal
					for (int i=0; i < subscribed.size; i++) {
						weak Subscriber s = subscribed.get(i);
						s.emit_changed(ret);
					}
					// TODO: Emit only for those who are intrested in the changed keys?
				}
			}

			static void CB_Connected(Location.GPSDevice device) {
				debug("Connected");
			}

			static void CB_Disconnected(Location.GPSDevice device) {
				debug("Disconnected");
			}

			void insert_double_key (string key, HashTable<string, TypedVariant?> ret, double double_value, bool do_insert) {
				Value v = Value (typeof(double));

				if (do_insert) {
					v.set_double(double_value);
					ret.insert (key, TypedVariant (ValueType.DOUBLE, v));
				}
				else {
					v.set_double (0);
					ret.insert (key, TypedVariant (ValueType.UNDETERMINED, v));
				}
			}


			public void Get (StringSet keys, HashTable<string, TypedVariant?> ret) {
				if (keys.is_disjoint (location_keys)) {
					return;
				}

				/*
				Note: If this is the first Get (and we don't start the service before this), the data may not be valid.
				*/

				// Copy the internally stored information to the map
				if (keys.is_member ("Context.Environment.Location.latitude")) {
					insert_double_key("Context.Environment.Location.latitude", ret, current_location.latitude, current_location.latlong_valid);
				}

				if (keys.is_member ("Context.Environment.Location.longitude")) {
					insert_double_key("Context.Environment.Location.longitude", ret, current_location.longitude, current_location.latlong_valid);
				}

				if (keys.is_member ("Context.Environment.Location.altitude")) {
					insert_double_key("Context.Environment.Location.altitude", ret, current_location.altitude, current_location.altitude_valid);
				}
			}

			public void Subscribe (StringSet keys, ContextKit.Subscriber s) {
				// FIXME: These are common to all plugins --> move to some upper level
				subscribed.add (s);
				subscribed_keys = new StringSet.union (subscribed_keys, keys);
				// FIXME: Also an intersection is needed.
			}

			public Key[] Keys() {
				return keys;
			}
		}
	}
}
