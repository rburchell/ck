/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

namespace Plugins {

	internal class HalPlugin : GLib.Object, ContextD.Plugin {

		private Hal.Context context;
		private string[] listenedDevices;

		private const string propertyChargeLevel = "battery.charge_level.percentage";
		private const string propertyIsCharging = "battery.rechargeable.is_charging";
		private const string propertyIsDischarging = "battery.rechargeable.is_discharging";

		private const string keyChargeLevel = "Context.Battery.ChargeLevel"; 
		/* Type: Integer from [0, 100]
		   Definition: True iff at least one battery is discharging.
		*/

		private const string keyOnBattery = "Context.Battery.OnBattery"; 
		/* Type: Boolean
		   Definition: True iff at least one battery is discharging.
		*/

		private const string keyLowBattery = "Context.Battery.LowBattery"; 
		/* Type: Boolean
		   Definition: True iff OnBattery is true and the charge percentages
		   of all batteries are below 10.
		*/

		private const string keyIsCharging = "Context.Battery.OnBattery"; 
		private const string keyTimeLeft = "Context.Battery.TimeLeft"; 
		private const string keyTimeUntilFull = "Context.Battery.TimeUntilFull"; 

		// TODO: Define and implement these


		// Constants for computation
		private int thresholdForLow = 10;

		HalPlugin () {
			listenedDevices = {};
		}

		~HalPlugin () {
		}
		
		bool install() {

			// Initialize collaboration with HAL
			context = new Hal.Context ();


			DBus.RawError error = DBus.RawError();
			var connection = DBus.RawBus.get(DBus.BusType.SYSTEM, ref error);
			bool success = context.set_dbus_connection(connection);

			if (!success) {
				debug ("Error: initializing HAL failed");
				return false;
			}				

			connection.setup_with_main (null); // Note: check

			error = DBus.RawError();

			// Note: this needs to be called after the dbus connection is set
			context.init (ref error);
			if (error.is_set ()) {
				debug ("Error: initializing HAL failed");
				return false;
			}

			// Set the callback which is called when device properties are modified
			success = context.set_device_property_modified (property_modified);

			if (!success) {
				debug ("Error: initializing HAL failed");
				return false;
			}				

			// Finally, install the plugin with libcontextprovider. 
			string[] keys = {"Context.Battery.Status", "Context.Battery.TimeLeft", "Context.Battery.ChargePercentage"};
			ContextProvider.install_group(keys, true, this.subscription_changed);

			return true;
		}

		private void subscription_changed (bool subscribe) {

			if (subscribe) {
				start_listening_devices ();
			}
			else {
				stop_listening_devices ();
			}
		}

		private void start_listening_devices () {
		
			// Read which battery devices are present
			DBus.RawError error = DBus.RawError ();
			var deviceNames = context.find_device_by_capability("battery", ref error);
	
			if (error.is_set()) {
				debug ("Error: finding devices failed");
				return;
			}

			if (deviceNames == null) {
				debug ("Error: finding devices failed");
				return;
			}

			foreach (var deviceName in deviceNames) {
				debug ("Found device %s", deviceName);

				// Start listening to property changes
				bool success = context.device_add_property_watch (deviceName, ref error);
				
				if (!success || error.is_set()) {
					debug ("Error: adding property watch failed");
					continue;
				}

				listenedDevices += deviceName;
			}

			if (listenedDevices.length == 1) {
				read_properties_one_device (listenedDevices[0]);
			}
		}

		private void stop_listening_devices () {
			
			foreach (var deviceName in listenedDevices) {
				DBus.RawError error = DBus.RawError ();
				context.device_remove_property_watch (deviceName, ref error);
			}
		
			listenedDevices = {};
		}
		
		private void property_modified (Hal.Context ctx, string udi, string key, bool is_removed, bool is_added) {
			debug ("Property modified: %s, %s", udi, key);

			read_properties_one_device (udi);

		}


		private void read_properties_one_device (string udi) {

			// Get ChargePercentage
			bool percentageKnown = false;
			int percentage;
			{
				DBus.RawError error = DBus.RawError ();
			
				percentage = context.device_get_property_int(udi, propertyChargeLevel, ref error);
		
				if (error.is_set()) {
					debug ("Error: querying property %s failed", propertyChargeLevel);
					ContextProvider.set_null (keyChargeLevel);

				} else {
					debug ("Read property: %s %d", propertyChargeLevel, percentage);
					percentageKnown = true;
					ContextProvider.set_integer (keyChargeLevel, percentage);
				}
			}

			// Get OnBattery
			bool discharging;
			bool dischargingKnown = false;
			{
				DBus.RawError error = DBus.RawError ();
				discharging = context.device_get_property_bool(udi, propertyIsDischarging, ref error);
		
				if (error.is_set()) {
					debug ("Error querying property %s", propertyIsCharging);
					ContextProvider.set_null (keyOnBattery);
				} else {
					debug ("Read property: %s %s", propertyIsCharging, (discharging ? "true" : "false"));
					dischargingKnown = true;
					ContextProvider.set_boolean (keyOnBattery, discharging);
				}
			}

			// Get LowBattery
			if (dischargingKnown && discharging == false) {
				// Not on battery -> low battery is false
				ContextProvider.set_boolean (keyLowBattery, false);
			}
			else if (percentageKnown && percentage >= thresholdForLow) {
				// Percentage high enough -> low battery is false
				ContextProvider.set_boolean (keyLowBattery, false);
			}
			else if (dischargingKnown && discharging == true && percentageKnown && percentage < thresholdForLow) {
				// Discharging and percentage is low
				ContextProvider.set_boolean (keyLowBattery, true);
			}
			else {
				// We don't have enough information to compute
				ContextProvider.set_null (keyLowBattery);
			}
		}
	}
}