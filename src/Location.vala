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

		public class Plugin : GLib.Object, ContextKit.Plugin {
			PluginMixins.SubscriptionList <Plugin> list;

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
			
			// Stores the last known location
			static LocationData current_location;
			
			// Device for using liblocation
			Location.GPSDevice device;
			

			void subscription_removed (Subscription s) {
				// TODO: Check here whether the connection to the loc service should be closed? 
				
				// FIXME: Do we have to remove it from the subscription list manually? Should the subscription list be part of the superclass?
			}
			
			
			/* Constructor
			Connect the liblocation signals to correct functions.
			Possibly: Indicate to liblocation that we're intrested in location data and start listening.
			*/
			public Plugin () {
			
				// Create plugin-related data structures
				list = new PluginMixins.SubscriptionList <Plugin> (this, subscription_removed);

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
				device = new Location.GPSDevice();
	
				// Connect signals from device
				device.changed += CB_Changed;
				device.connected += CB_Connected;
				device.disconnected += CB_Disconnected;
				
				device.start(); // FIXME: This is only for testing
							
				// FIXME: Do we need to unref control? Or does Vala handle it automatically?
				
				
			}
			
			// Handling signals from GPS control
			static void CB_Error(Location.GPSDControl? control) {
				stdout.printf("Error");
			}
			
			static void CB_Running(Location.GPSDControl? control) {
				stdout.printf("Running");
			}
			
			static void CB_Stopped(Location.GPSDControl? control) {
				stdout.printf("Stopped");
			}
			
			// Handling signals from GPS device
			static void CB_Changed(Location.GPSDevice? device) {
				stdout.printf("Changed %f %f %f", device.fix.latitude, device.fix.longitude, device.fix.altitude);
				
				if (device != null && device.fix != null) { 
				
					if ((device.fix.fields & Location.GPS_DEVICE_LATLONG_SET) != 0) {
						current_location.latitude = device.fix.latitude;
						current_location.longitude = device.fix.longitude;
						current_location.latlong_valid = true;
					}
					
					if ((device.fix.fields & Location.GPS_DEVICE_ALTITUDE_SET) != 0) {
						current_location.altitude = device.fix.altitude;
						current_location.altitude_valid = true;
					} 
					
					// TODO: Replace the valid / non-valid boolean with a time stamp		  
				}
				
				// TODO: Should we check whether we need the location service any more?
				// If we stop the connecton, we won't be able to answer future Get:s accurately.
				
				// TODO: How is the "Changed" signal sent?
			}
			
			static void CB_Connected(Location.GPSDevice device) {
				stdout.printf("Connected");
			}
			
			static void CB_Disconnected(Location.GPSDevice device) {
				stdout.printf("Disconnected");
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

			public void Subscribe (StringSet keys, ContextKit.Subscription s) {
				
			}

			public Key[] Keys() {
				return keys;
			}
		}
	}
}
