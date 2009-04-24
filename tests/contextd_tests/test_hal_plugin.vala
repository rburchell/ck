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

const string halPercentage = "battery.charge_level.percentage";
const string halCharge = "battery.charge_level.current";
const string halLastFull = "battery.charge_level.last_full";
const string halRate = "battery.charge_level.rate";
const string halCharging = "battery.rechargeable.is_charging";
const string halDischarging = "battery.rechargeable.is_discharging";

const string keyChargePercentage = "Context.Battery.ChargePercentage"; 
const string keyOnBattery = "Context.Battery.OnBattery"; 
const string keyLowBattery = "Context.Battery.LowBattery"; 
const string keyIsCharging = "Context.Battery.IsCharging"; 
const string keyTimeUntilLow = "Context.Battery.TimeUntilLow"; 
const string keyTimeUntilFull = "Context.Battery.TimeUntilFull"; 

void test_install() {
	// Setup
	ContextProvider.initializeMock();
	Hal.initializeMock();

	// Test
	Plugins.HalPlugin plugin = new Plugins.HalPlugin();
	bool success = plugin.install ();

	// Expected results:
	// the installation succeeded
	assert (success == true);
	// the plugin declares the correct keys
	Gee.HashSet<string> keys = ContextProvider.providedKeys;
	assert (keys.contains (keyChargePercentage));
	assert (keys.contains (keyOnBattery));
	assert (keys.contains (keyLowBattery));
	assert (keys.contains (keyIsCharging));
	assert (keys.contains (keyTimeUntilLow));
	assert (keys.contains (keyTimeUntilFull));
}

void test_chargepercentage () {
	// Setup
	ContextProvider.initializeMock ();
	Hal.initializeMock ();
	// Set the initial values for the properties
	Hal.changePropertyInt (halPercentage, 50, false);
	Hal.changePropertyInt (halCharge, 100, false);
	Hal.changePropertyInt (halLastFull, 200, false);
	Hal.changePropertyInt (halRate, 0, false);
	Hal.changePropertyBool (halCharging, false, false);
	Hal.changePropertyBool (halDischarging, false, false);

	Plugins.HalPlugin plugin = new Plugins.HalPlugin();
	plugin.install ();
	// tell the plugin that someone is listening
	ContextProvider.callSubscriptionCallback(true);

	ContextProvider.resetValues ();
	// Test
	// Command libhal mock implementation to change a value
	// of a property and to notify the client of the change
	Hal.changePropertyInt(halPercentage, 53, true);

	// Expected results: 
	Gee.HashMap<string, int> intValues = ContextProvider.intValues;
	assert (intValues.get_keys().contains(keyChargePercentage));
	assert (intValues.get(keyChargePercentage) == 53);
}

void test_onbattery () {
	// Setup
	ContextProvider.initializeMock ();
	Hal.initializeMock ();
	// Set the initial values for the properties
	Hal.changePropertyInt (halPercentage, 50, false);
	Hal.changePropertyInt (halCharge, 100, false);
	Hal.changePropertyInt (halLastFull, 200, false);
	Hal.changePropertyInt (halRate, 0, false);
	Hal.changePropertyBool (halCharging, false, false);
	Hal.changePropertyBool (halDischarging, false, false);

	Plugins.HalPlugin plugin = new Plugins.HalPlugin();
	plugin.install ();
	// tell the plugin that someone is listening
	ContextProvider.callSubscriptionCallback(true);

	// Test
	// Command libhal mock implementation to change a value
	// of a property and to notify the client of the change
	ContextProvider.resetValues ();
	Hal.changePropertyBool(halDischarging, true, true);

	// Expected results:
	Gee.HashMap<string, bool> boolValues = ContextProvider.boolValues;
	assert (boolValues.get_keys().contains(keyOnBattery));
	assert (boolValues.get(keyOnBattery) == true);

	// Test
	// Command libhal mock implementation to change a value
	// of a property and to notify the client of the change
	ContextProvider.resetValues ();
	Hal.changePropertyBool(halDischarging, false, true);

	// Expected results:
	boolValues = ContextProvider.boolValues;
	assert (boolValues.get_keys().contains(keyOnBattery));
	assert (boolValues.get(keyOnBattery) == false);
}

void test_lowbattery () {
	// Setup
	ContextProvider.initializeMock ();
	Hal.initializeMock ();
	// Set the initial values for the properties
	Hal.changePropertyInt (halPercentage, 50, false);
	Hal.changePropertyInt (halCharge, 100, false);
	Hal.changePropertyInt (halLastFull, 200, false);
	Hal.changePropertyInt (halRate, 0, false);
	Hal.changePropertyBool (halCharging, false, false);
	Hal.changePropertyBool (halDischarging, false, false);

	Plugins.HalPlugin plugin = new Plugins.HalPlugin();
	plugin.install ();
	// tell the plugin that someone is listening
	ContextProvider.callSubscriptionCallback(true);

	// Test
	// Command libhal mock implementation to change a value
	// of a property and to notify the client of the change
	ContextProvider.resetValues ();
	Hal.changePropertyBool (halDischarging, true, true);
	Hal.changePropertyInt(halPercentage, 5, true);
	// Note: if we set more than one properties, we need to call the
	// property_modified callback for both of them (last parameter: true)

	// Expected results:
	Gee.HashMap<string, bool> boolValues = ContextProvider.boolValues;
	assert (boolValues.get_keys().contains(keyLowBattery));
	assert (boolValues.get(keyLowBattery) == true);

	// Test
	// Command libhal mock implementation to change a value
	// of a property and to notify the client of the change
	ContextProvider.resetValues ();
	Hal.changePropertyBool (halDischarging, true, false);
	Hal.changePropertyInt(halPercentage, 15, true);

	// Expected results:
	boolValues = ContextProvider.boolValues;
	assert (boolValues.get_keys().contains(keyLowBattery));
	assert (boolValues.get(keyLowBattery) == false);

	// Test
	// Command libhal mock implementation to change a value
	// of a property and to notify the client of the change
	ContextProvider.resetValues ();
	Hal.changePropertyBool (halDischarging, false, true);
	Hal.changePropertyInt(halPercentage, 5, true);

	// Expected results:
	boolValues = ContextProvider.boolValues;
	assert (boolValues.get_keys().contains(keyLowBattery));
	assert (boolValues.get(keyLowBattery) == false);
}

void test_ischarging () {
	// Setup
	ContextProvider.initializeMock ();
	Hal.initializeMock ();
	// Set the initial values for the properties
	Hal.changePropertyInt (halPercentage, 50, false);
	Hal.changePropertyInt (halCharge, 100, false);
	Hal.changePropertyInt (halLastFull, 200, false);
	Hal.changePropertyInt (halRate, 0, false);
	Hal.changePropertyBool (halCharging, false, false);
	Hal.changePropertyBool (halDischarging, false, false);

	Plugins.HalPlugin plugin = new Plugins.HalPlugin();
	plugin.install ();
	// tell the plugin that someone is listening
	ContextProvider.callSubscriptionCallback(true);

	// Test
	// Command libhal mock implementation to change a value
	// of a property and to notify the client of the change
	ContextProvider.resetValues ();
	Hal.changePropertyBool(halCharging, true, true);

	// Expected results:
	Gee.HashMap<string, bool> boolValues = ContextProvider.boolValues;
	assert (boolValues.get_keys().contains(keyIsCharging));
	assert (boolValues.get(keyIsCharging) == true);

	// Test
	// Command libhal mock implementation to change a value
	// of a property and to notify the client of the change
	ContextProvider.resetValues ();
	Hal.changePropertyBool(halCharging, false, true);

	// Expected results:
	boolValues = ContextProvider.boolValues;
	assert (boolValues.get_keys().contains(keyIsCharging));
	assert (boolValues.get(keyIsCharging) == false);
}

void test_timeuntillow () {
	// Setup
	ContextProvider.initializeMock ();
	Hal.initializeMock ();
	// Set the initial values for the properties
	Hal.changePropertyInt (halPercentage, 50, false);
	Hal.changePropertyInt (halCharge, 100, false);
	Hal.changePropertyInt (halLastFull, 200, false);
	Hal.changePropertyInt (halRate, 0, false);
	Hal.changePropertyBool (halCharging, false, false);
	Hal.changePropertyBool (halDischarging, false, false);

	Plugins.HalPlugin plugin = new Plugins.HalPlugin();
	plugin.install ();
	// tell the plugin that someone is listening
	ContextProvider.callSubscriptionCallback(true);

	// Test
	// Command libhal mock implementation to change a value
	// of a property and to notify the client of the change
	ContextProvider.resetValues ();
	Hal.changePropertyBool(halDischarging, true, true);
	Hal.changePropertyInt(halCharge, 7000, true);
	Hal.changePropertyInt(halLastFull, 10000, true);
	Hal.changePropertyInt(halRate, 1000, true);

	// Expected results:
	Gee.HashMap<string, int> intValues = ContextProvider.intValues;
	assert (intValues.get_keys().contains(keyTimeUntilLow));

	// Takes 6 hours to reach battery 10%
	assert (intValues.get(keyTimeUntilLow) == 6 * 3600);

	// Test
	// Command libhal mock implementation to change a value
	// of a property and to notify the client of the change
	ContextProvider.resetValues ();
	Hal.changePropertyBool(halDischarging, true, true);
	Hal.changePropertyInt(halCharge, 5, true); // Already low!
	Hal.changePropertyInt(halLastFull, 100, true);
	Hal.changePropertyInt(halRate, 3, true);

	// Expected results:
	intValues = ContextProvider.intValues;
	assert (intValues.get_keys().contains(keyTimeUntilLow));
	assert (intValues.get(keyTimeUntilLow) == 0);
}

void test_timeuntilfull () {
	// Setup
	ContextProvider.initializeMock ();
	Hal.initializeMock ();
	// Set the initial values for the properties
	Hal.changePropertyInt (halPercentage, 50, false);
	Hal.changePropertyInt (halCharge, 100, false);
	Hal.changePropertyInt (halLastFull, 200, false);
	Hal.changePropertyInt (halRate, 0, false);
	Hal.changePropertyBool (halCharging, false, false);
	Hal.changePropertyBool (halDischarging, false, false);

	Plugins.HalPlugin plugin = new Plugins.HalPlugin();
	plugin.install ();
	// tell the plugin that someone is listening
	ContextProvider.callSubscriptionCallback(true);

	// Test
	// Command libhal mock implementation to change a value
	// of a property and to notify the client of the change
	ContextProvider.resetValues ();
	Hal.changePropertyBool(halCharging, true, true);
	Hal.changePropertyInt(halCharge, 7000, true);
	Hal.changePropertyInt(halLastFull, 10000, true);
	Hal.changePropertyInt(halRate, 1000, true);

	// Expected results:
	Gee.HashMap<string, int> intValues = ContextProvider.intValues;
	assert (intValues.get_keys().contains(keyTimeUntilFull));
	assert (intValues.get(keyTimeUntilFull) == 3 * 3600);

	// Test
	// Command libhal mock implementation to change a value
	// of a property and to notify the client of the change
	ContextProvider.resetValues ();
	Hal.changePropertyBool(halCharging, true, true);
	Hal.changePropertyInt(halCharge, 100, true); // Already full
	Hal.changePropertyInt(halLastFull, 100, true);
	Hal.changePropertyInt(halRate, 3, true);

	// Expected results:
	intValues = ContextProvider.intValues;
	assert (intValues.get_keys().contains(keyTimeUntilFull));
	assert (intValues.get(keyTimeUntilFull) == 0);
}


void debug_null  (string? log_domain, LogLevelFlags log_level, string message)
{
}

public static void main (string[] args) {
	Test.init (ref args);

	if (Test.quiet()) {
		//Log.set_handler ("ContextKit", LogLevelFlags.LEVEL_DEBUG | LogLevelFlags.FLAG_RECURSION, debug_null);
	}

	Test.add_func("/contextd/hal_plugin/test_install", test_install);
	Test.add_func("/contextd/hal_plugin/test_chargepercentage", test_chargepercentage);
	Test.add_func("/contextd/hal_plugin/test_onbattery", test_onbattery);
	Test.add_func("/contextd/hal_plugin/test_lowbattery", test_lowbattery);
	Test.add_func("/contextd/hal_plugin/test_ischarging", test_ischarging);
	Test.add_func("/contextd/hal_plugin/test_timeuntillow", test_timeuntillow);
	Test.add_func("/contextd/hal_plugin/test_timeuntilfull", test_timeuntilfull);
	Test.run ();
}
