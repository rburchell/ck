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

		private struct BatteryInfo {
			public int percentage;
			public bool percentageKnown;
			public int charge;
			public bool chargeKnown;
			public int lastFull;
			public bool lastFullKnown;
			public int lastNonzeroChargingRate;
			public int lastNonzeroDischargingRate;
			public bool charging;
			public bool chargingKnown;
			public bool discharging;
			public bool dischargingKnown;
		}

		private static Gee.HashMap<string, BatteryInfo?> batteries;

		private static Hal.Context? halContext;

		// Property names used by HAL
		private const string halPercentage = "battery.charge_level.percentage";
		private const string halCharge = "battery.charge_level.current";
		private const string halLastFull = "battery.charge_level.last_full";
		private const string halRate = "battery.charge_level.rate";

		private const string halCharging = "battery.rechargeable.is_charging";
		private const string halDischarging = "battery.rechargeable.is_discharging";

		// Context keys provided by this plugin
		private const string keyChargePercentage = "Context.Battery.ChargePercentage"; 
		private const string keyOnBattery = "Context.Battery.OnBattery"; 
		private const string keyLowBattery = "Context.Battery.LowBattery"; 

		private const string keyTimeUntilLow = "Context.Battery.TimeUntilLow"; 
		private const string keyTimeUntilFull = "Context.Battery.TimeUntilFull"; 

		// TODO: Define and implement these


		// Constants for computation
		private static const int thresholdForLow = 10;

		internal HalPlugin () {
			batteries = new Gee.HashMap<string, BatteryInfo?>(str_hash, str_equal);
		}

		~HalPlugin () {
		}
		
		bool install() {

			// Initialize collaboration with HAL
			halContext = new Hal.Context ();

			DBus.RawError error = DBus.RawError();
			var connection = DBus.RawBus.get(DBus.BusType.SYSTEM, ref error);
			bool success = halContext.set_dbus_connection(connection);

			if (!success) {
				debug ("Error: initializing HAL failed");
				return false;
			}				

			connection.setup_with_main (null);

			error = DBus.RawError();

			// Note: this needs to be called after the dbus connection is set
			halContext.init (ref error);
			if (error.is_set ()) {
				debug ("Error: initializing HAL failed");
				return false;
			}

			// Set the callback which is called when device properties are modified
			success = halContext.set_device_property_modified (property_modified);

			if (!success) {
				debug ("Error: initializing HAL failed");
				return false;
			}				

			// Finally, install the plugin with libcontextprovider. 
			string[] keys = {keyChargePercentage, keyOnBattery, keyLowBattery, keyTimeUntilLow, keyTimeUntilFull};
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
			var deviceNames = halContext.find_device_by_capability("battery", ref error);
	
			if (error.is_set()) {
				debug ("Error: finding devices failed");
				return;
			}

			if (deviceNames == null) {
				debug ("Error: finding devices failed");
				return;
			}

			foreach (var udi in deviceNames) {
				debug ("Found device %s", udi);

				// Start listening to property changes
				bool success = halContext.device_add_property_watch (udi, ref error);
				
				if (!success || error.is_set()) {
					debug ("Error: adding property watch failed");
				}

				// Read initial values
				BatteryInfo info = BatteryInfo();
				readIntProperty (udi, halPercentage, ref info.percentage, ref info.percentageKnown);
				readIntProperty (udi, halCharge, ref info.charge, ref info.chargeKnown);
				readIntProperty (udi, halLastFull, ref info.lastFull, ref info.lastFullKnown);
				readBoolProperty (udi, halCharging, ref info.charging, ref info.chargingKnown);
				readBoolProperty (udi, halDischarging, ref info.discharging, ref info.dischargingKnown);

				int rate = 0;
				bool rateKnown = false;
				readIntProperty (udi, halRate, ref rate, ref rateKnown);
				// If we got an informative rate (known and >0), record it
				// for future use.
				if (rateKnown && rate > 0) {

					if (info.chargingKnown && info.charging) {
						info.lastNonzeroChargingRate = rate;
					}

					if (info.dischargingKnown && info.discharging) {
						info.lastNonzeroDischargingRate = rate;
					}
					
				}

				batteries.set (udi, info);
			}
			calculateProperties ();
		}

		// Stops listening property modifications
		private void stop_listening_devices () {
			
			foreach (var udi in batteries.get_keys ()) {
				DBus.RawError error = DBus.RawError ();
				halContext.device_remove_property_watch (udi, ref error);
			}
		
			batteries.clear ();
		}
		
		// Is called when HAL notifies us that a property has changed
		private static void property_modified (Hal.Context ctx, string udi, string key, bool is_removed, bool is_added) {
			debug ("Property modified: %s, %s", udi, key);

			BatteryInfo info = batteries.get (udi);

			if (key == halPercentage) {
				readIntProperty(udi, key, ref info.percentage, ref info.percentageKnown);
			}
			else if (key == halCharge) {
				readIntProperty(udi, key, ref info.charge, ref info.chargeKnown);
			}
			else if (key == halLastFull) {
				readIntProperty(udi, key, ref info.lastFull, ref info.lastFullKnown);
			}
			else if (key == halRate || key == halCharging || key == halDischarging) {
				// The information about charging / discharging and the rate are
				// coupled together, so, read them all at once.

				readBoolProperty(udi, halCharging, ref info.charging, ref info.chargingKnown);
				readBoolProperty(udi, halDischarging, ref info.discharging, ref info.dischargingKnown);

				int rate = 0;
				bool rateKnown = false;
				readIntProperty (udi, halRate, ref rate, ref rateKnown);
				// If we got an informative rate (known and >0), record it
				// for future use.
				if (rateKnown && rate > 0) {

					if (info.chargingKnown && info.charging) {
						info.lastNonzeroChargingRate = rate;
					}

					if (info.dischargingKnown && info.discharging) {
						info.lastNonzeroDischargingRate = rate;
					}
					
				}
			}
			else {
				// Property not interesting for us
				return;
			}

			batteries.set (udi, info);
			calculateProperties ();
		}

		// Reads an int property from libhal and records whether it was successful
		private static void readIntProperty(string udi, string property, ref int value, ref bool success) {
				success = false;
				DBus.RawError error = DBus.RawError ();

				value = halContext.device_get_property_int(udi, property, ref error);
		
				if (error.is_set()) {
					debug ("Error: querying property %s failed", property);
				} else {
					debug ("Read property: %s %d", property, value);
					success = true;
				}
		}

		// Reads a bool property from libhal and records whether it was successful
		private static void readBoolProperty(string udi, string property, ref bool value, ref bool success) {
				success = false;
				DBus.RawError error = DBus.RawError ();

				value = halContext.device_get_property_bool(udi, property, ref error);
		
				if (error.is_set()) {
					debug ("Error: querying property %s failed", property);
				} else {
					debug ("Read property: %s %s", property, value ? "true" : "false");
					success = true;
				}
		}
		
		// Calculates the values of the context properties and declares them
		// using the libcontextprovider. The current implementation assumes that
		// there is only one battery. If support for multiple batteries is needed,
		// we need to define more specifically when to set the properties unknown,
		// e.g. is the average charge level unknown if one of the charge levels 
		// is unknown or only when all of them are unknown.
		private static void calculateProperties () {

			debug ("Calculating properties");
			if (batteries.size != 1) {
				debug ("Not supported: no batteries / more than one battery");
			}

			var udis = batteries.get_keys ();

			string udi = "";
			foreach (var temp in batteries.get_keys ()) {
				debug ("battery %s", temp);
				udi = temp;
			}

			BatteryInfo info = batteries.get (udi);


			debug ("%d %d %d %d %s %s", info.percentage, info.charge, info.lastNonzeroChargingRate, info.lastFull, info.charging? "true" : "false", info.discharging? "true" : "false");

			// Compute ChargePercentage
			if (info.percentageKnown == false) {
				debug ("percentage unknown");
				ContextProvider.set_null (keyChargePercentage);
			}
			else {
				debug ("percentage %d",info.percentage);
				ContextProvider.set_integer (keyChargePercentage, info.percentage);
			}

			// Compute OnBattery
			if (info.dischargingKnown == false) {
				debug ("onbattery unknown");
				ContextProvider.set_null (keyOnBattery);
			}
			else {
				debug ("onbattery %s", info.discharging ? "true" : "false");
				ContextProvider.set_boolean (keyOnBattery, info.discharging);
			}

			// Compute LowBattery
			if (info.dischargingKnown && info.discharging == false) {
				// Not on battery -> low battery is false
				ContextProvider.set_boolean (keyLowBattery, false);
			}
			else if (info.percentageKnown && info.percentage >= thresholdForLow) {
				// Percentage high enough -> low battery is false
				ContextProvider.set_boolean (keyLowBattery, false);
			}
			else if (info.dischargingKnown && info.discharging == true && info.percentageKnown && info.percentage < thresholdForLow) {
				// Discharging and percentage is low
				ContextProvider.set_boolean (keyLowBattery, true);
			}
			else {
				// We don't have enough information to compute
				ContextProvider.set_null (keyLowBattery);
			}
			// Note that the logic of LowBattery is different when there are 
			// multiple devices:
			// we need to check against the "global" OnBattery (if some device
			// is on battery) and not the OnBattery of this particular device.

			// Compute TimeUntilLow
			if (info.lastNonzeroDischargingRate > 0 && info.chargeKnown && info.lastFullKnown) {
				double timeUntilLow = (1.0 * info.charge - thresholdForLow / 100.0 * info.lastFull) / info.lastNonzeroDischargingRate;

				timeUntilLow *= 60; // conversion to minutes
				timeUntilLow = Math.round(timeUntilLow); // round to nearest minute
				timeUntilLow *= 60; // conversion to seconds

				ContextProvider.set_integer (keyTimeUntilLow, (int) timeUntilLow);
			}
			else {
				ContextProvider.set_null (keyTimeUntilLow);
			}

			// Compute TimeUntilFull
			if (info.lastNonzeroChargingRate > 0 && info.chargeKnown && info.lastFullKnown) {
				double timeUntilFull = (1.0 * info.lastFull - info.charge) / info.lastNonzeroChargingRate; 

				timeUntilFull *= 60; // conversion to minutes
				timeUntilFull = Math.round(timeUntilFull); // round to nearest minute
				timeUntilFull *= 60; // conversion to seconds


				ContextProvider.set_integer (keyTimeUntilFull, (int) timeUntilFull);
			}
			else {
				ContextProvider.set_null (keyTimeUntilFull);
			}

		}
	}
}