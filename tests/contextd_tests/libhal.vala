namespace Hal {

	public class Context {
		public static delegate void DevicePropertyModified (Context ctx, string udi, string key, bool is_removed, bool is_added);
	
		public bool init (ref DBus.RawError error) {
			return true;
		}

		public bool set_dbus_connection (DBus.RawConnection conn) {
			return true;
		}

		public bool set_device_property_modified (DevicePropertyModified _callback) {
			return true;
		}

		public string[] find_device_by_capability (string capability, ref DBus.RawError error) {
			string [] toReturn = {"fake.battery"};
			return toReturn;
		}

		public int device_get_property_int (string udi, string key, ref DBus.RawError error) {
			return 0;
		}

		public bool device_get_property_bool (string udi, string key, ref DBus.RawError error) {
			return false;
		}

		public bool device_add_property_watch (string udi, ref DBus.RawError error) {
			return true;
		}

		public bool device_remove_property_watch (string udi, ref DBus.RawError error) {
			return true;
		}
	}
}