using GLib;

namespace ContextProvider {

	/* All because GValue SUCKS. bring on GVariant */
	bool value_compare (Value? v1, Value? v2) {
		if (v1 == null && v2 == null)
			return true;
		if (v1 == null || v2 == null)
			return false;
		if (v1.type() == typeof(int) && v2.type() == typeof(int))
			return (v1.get_int() == v2.get_int());
		if (v1.type() == typeof(double) && v2.type() == typeof(double))
			return (v1.get_double() == v2.get_double());
		if (v1.type() == typeof(bool) && v2.type() == typeof(bool))
			return (v1.get_boolean() == v2.get_boolean());
		if (v1.type() == typeof(string) && v2.type() == typeof(string))
			return (v1.get_string() == v2.get_string());
		return false;
	}
}
