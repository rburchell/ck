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
			public bool valid;
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
				list = new PluginMixins.SubscriptionList <Plugin> (this, subscription_removed);

				location_keys = new StringSet.from_array (new string[] {
						"Context.Environment.Location.latitude",
						"Context.Environment.Location.longitude",
						"Context.Environment.Location.altitude"
						});
				
				// Fetch an instance of the LocationGPSDControl
				Location.GPSDControl control;
				control = control.get_default();
				
				// Connect signals from LocationGPSDControl
				control.error += CB_Error;
	            control.gpsd_running += CB_Running;
	            control.gpsd_stopped += CB_Stopped;
	            
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
                
                if (!isnull(device) && !isnull(device.fix)) { 
                
                    if (device.fix.fields & LOCATION_GPS_DEVICE_LATLONG_SET) {
                        current_location.latitude = device.fix.latitude;
                        current_location.longitude = device.fix.longitude;
                    }
                    
                    if (device.fix.fields & LOCATION_GPS_DEVICE_ALTITUDE_SET) {
                        current_location.altitude = device.fix.altitude;
                    } 
                    //current_location.fields = device->fix->fields;
                    
                    // How to invalidate the data?              
                }
                
                // If we got the data and if there are no subscribers, 
                // indicate to liblocation that we are no longer intrested in 
                // location information.
		    
		    }
		    
		    static void CB_Connected(Location.GPSDevice device) {
		        stdout.printf("Connected");
		    }
		    
		    static void CB_Disconnected(Location.GPSDevice device) {
		        stdout.printf("Disconnected");
		    }


			public void Get (StringSet keys, HashTable<string, TypedVariant?> ret) {
				if (keys.is_disjoint (location_keys)) {
					return;
			 	}
				
				// Make sure that the location service is started
				
				
				/*LocationGPSDControl* control;
	            control = location_gpsd_control_get_default();
	
	            location_gpsd_control_start(control);*/
				
				// FIXME: If this is the first get, the data is probably not available yet. Autsch! What to do?
				
				
				// FIXME: What if the data is not available yet?
				
				if (current_location.valid) {
				    // Copy the stored information from the struct to the map
				    
				    if (keys.is_member ("Context.Environment.Location.latitude")) {
				        Value val = Value (typeof(double));
				        val.set_double(current_location.latitude);
				        ret.insert ("Context.Environment.Location.latitude", TypedVariant (ValueType.DOUBLE, val));
				    }
				    
				    /*if (keys.is_member ("Context.Environment.Location.longitude")) {
				        ret.insert ("Context.Environment.Location.longitude", TypedVariant (ValueType.DOUBLE, current_location.longitude));
				    }
				    
				    if (keys.is_member ("Context.Environment.Location.altitude")) {
				        ret.insert ("Context.Environment.Location.altitude", TypedVariant (ValueType.DOUBLE, current_location.longitude));
				    }*/
				}
				// FIXME: change do double
			}

			public void Subscribe (StringSet keys, ContextKit.Subscription s) {
			    // TODO: Start the gps here? Does it work if we create the "device" before it?
			}

			public Key[] Keys() {
				return keys;
			}
		}
	}
}
