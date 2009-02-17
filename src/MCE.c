
#include <src/MCE.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib.h>
#include <gee/iterable.h>
#include <gee/iterator.h>
#include <gee/collection.h>
#include <stdio.h>
#include <manager.h>
#include <key_usage_counter.h>
#include <string.h>




struct _ContextKitMCEProviderPrivate {
	DBusGConnection* conn;
	DBusGProxy* mce_request;
	DBusGProxy* mce_signal;
	ContextProviderStringSet* all_keys;
	ContextProviderStringSet* orientation_keys;
	ContextProviderStringSet* display_status_keys;
	ContextProviderStringSet* device_mode_keys;
	ContextProviderStringSet* call_state_keys;
	ContextProviderStringSet* inactivity_status_keys;
};

#define CONTEXT_KIT_MCE_PROVIDER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), CONTEXT_KIT_MCE_TYPE_PROVIDER, ContextKitMCEProviderPrivate))
enum  {
	CONTEXT_KIT_MCE_PROVIDER_DUMMY_PROPERTY
};
static void context_kit_mce_provider_error_for_subset (ContextKitMCEProvider* self, ContextProviderStringSet* keys, GHashTable* ret, GeeArrayList* unavailable_keys, ContextProviderStringSet* intersect_with);
static gint context_kit_mce_provider_abs (ContextKitMCEProvider* self, gint n);
static gint context_kit_mce_provider_calculate_orientation (ContextKitMCEProvider* self, gint x, gint y);
void _dynamic_get_device_orientation1 (DBusGProxy* self, char** param1, char** param2, char** param3, gint* param4, gint* param5, gint* param6, GError** error);
static void context_kit_mce_provider_get_orientation (ContextKitMCEProvider* self, ContextProviderStringSet* keys, GHashTable* ret, GeeArrayList* unavail);
void _dynamic_get_display_status2 (DBusGProxy* self, char** param1, GError** error);
static void context_kit_mce_provider_get_display_status (ContextKitMCEProvider* self, ContextProviderStringSet* keys, GHashTable* ret, GeeArrayList* unavail);
void _dynamic_get_device_mode3 (DBusGProxy* self, char** param1, GError** error);
static void context_kit_mce_provider_get_device_mode (ContextKitMCEProvider* self, ContextProviderStringSet* keys, GHashTable* ret, GeeArrayList* unavail);
void _dynamic_get_call_state4 (DBusGProxy* self, char** param1, char** param2, GError** error);
static void context_kit_mce_provider_get_call_state (ContextKitMCEProvider* self, ContextProviderStringSet* keys, GHashTable* ret, GeeArrayList* unavail);
void _dynamic_get_inactivity_status5 (DBusGProxy* self, gboolean* param1, GError** error);
static void context_kit_mce_provider_get_inactivity_status (ContextKitMCEProvider* self, ContextProviderStringSet* keys, GHashTable* ret, GeeArrayList* unavail);
static GValue* _g_value_dup (GValue* self);
static void context_kit_mce_provider_insert_orientation_to_map (ContextKitMCEProvider* self, ContextProviderStringSet* keys, const ContextKitMCEDeviceOrientation* orientation, GHashTable* ret, GeeArrayList* unavail);
static void context_kit_mce_provider_insert_display_status_to_map (ContextKitMCEProvider* self, ContextProviderStringSet* keys, const char* display_status, GHashTable* ret, GeeArrayList* unavail);
static void context_kit_mce_provider_insert_device_mode_to_map (ContextKitMCEProvider* self, ContextProviderStringSet* keys, const char* device_mode, GHashTable* ret, GeeArrayList* unavail);
static void context_kit_mce_provider_insert_call_state_to_map (ContextKitMCEProvider* self, ContextProviderStringSet* keys, const char* state, const char* type, GHashTable* ret, GeeArrayList* unavail);
static void context_kit_mce_provider_insert_inactivity_status_to_map (ContextKitMCEProvider* self, ContextProviderStringSet* keys, gboolean status, GHashTable* ret, GeeArrayList* unavail);
static void context_kit_mce_provider_orientation_changed (ContextKitMCEProvider* self, DBusGProxy* sender, const char* rotation, const char* stand, const char* facing, gint32 x, gint32 y, gint32 z);
static void context_kit_mce_provider_display_status_changed (ContextKitMCEProvider* self, DBusGProxy* sender, const char* display_status);
static void context_kit_mce_provider_device_mode_changed (ContextKitMCEProvider* self, DBusGProxy* sender, const char* device_mode);
static void context_kit_mce_provider_call_state_changed (ContextKitMCEProvider* self, DBusGProxy* sender, const char* state, const char* type);
static void context_kit_mce_provider_inactivity_status_changed (ContextKitMCEProvider* self, DBusGProxy* sender, gboolean status);
static void context_kit_mce_provider_real_get (ContextProviderProvider* base, ContextProviderStringSet* keys, GHashTable* ret, GeeArrayList* unavail);
static void context_kit_mce_provider_ensure_mce_signal_exists (ContextKitMCEProvider* self);
static void _context_kit_mce_provider_orientation_changed_dynamic_sig_device_orientation_ind0_ (DBusGProxy* _sender, const char* rotation, const char* stand, const char* facing, gint32 x, gint32 y, gint32 z, gpointer self);
void _dynamic_sig_device_orientation_ind1_connect (gpointer obj, const char * signal_name, GCallback handler, gpointer data);
static void _context_kit_mce_provider_display_status_changed_dynamic_display_status_ind2_ (DBusGProxy* _sender, const char* display_status, gpointer self);
void _dynamic_display_status_ind3_connect (gpointer obj, const char * signal_name, GCallback handler, gpointer data);
static void _context_kit_mce_provider_device_mode_changed_dynamic_sig_device_mode_ind4_ (DBusGProxy* _sender, const char* device_mode, gpointer self);
void _dynamic_sig_device_mode_ind5_connect (gpointer obj, const char * signal_name, GCallback handler, gpointer data);
static void _context_kit_mce_provider_call_state_changed_dynamic_sig_call_state_ind6_ (DBusGProxy* _sender, const char* state, const char* type, gpointer self);
void _dynamic_sig_call_state_ind7_connect (gpointer obj, const char * signal_name, GCallback handler, gpointer data);
static void _context_kit_mce_provider_inactivity_status_changed_dynamic_system_inactivity_ind8_ (DBusGProxy* _sender, gboolean status, gpointer self);
void _dynamic_system_inactivity_ind9_connect (gpointer obj, const char * signal_name, GCallback handler, gpointer data);
static void context_kit_mce_provider_real_keys_subscribed (ContextProviderProvider* base, ContextProviderStringSet* keys);
static void _context_kit_mce_provider_orientation_changed_dynamic_sig_device_orientation_ind10_ (DBusGProxy* _sender, const char* rotation, const char* stand, const char* facing, gint32 x, gint32 y, gint32 z, gpointer self);
void _dynamic_sig_device_orientation_ind11_disconnect (gpointer obj, const char * signal_name, GCallback handler, gpointer data);
static void _context_kit_mce_provider_display_status_changed_dynamic_display_status_ind12_ (DBusGProxy* _sender, const char* display_status, gpointer self);
void _dynamic_display_status_ind13_disconnect (gpointer obj, const char * signal_name, GCallback handler, gpointer data);
static void _context_kit_mce_provider_device_mode_changed_dynamic_sig_device_mode_ind14_ (DBusGProxy* _sender, const char* device_mode, gpointer self);
void _dynamic_sig_device_mode_ind15_disconnect (gpointer obj, const char * signal_name, GCallback handler, gpointer data);
static void _context_kit_mce_provider_call_state_changed_dynamic_sig_call_state_ind16_ (DBusGProxy* _sender, const char* state, const char* type, gpointer self);
void _dynamic_sig_call_state_ind17_disconnect (gpointer obj, const char * signal_name, GCallback handler, gpointer data);
static void _context_kit_mce_provider_inactivity_status_changed_dynamic_system_inactivity_ind18_ (DBusGProxy* _sender, gboolean status, gpointer self);
void _dynamic_system_inactivity_ind19_disconnect (gpointer obj, const char * signal_name, GCallback handler, gpointer data);
static void context_kit_mce_provider_real_keys_unsubscribed (ContextProviderProvider* base, ContextProviderStringSet* keys, ContextProviderStringSet* keys_remaining);
static ContextProviderStringSet* context_kit_mce_provider_real_provided_keys (ContextProviderProvider* base);
static gpointer context_kit_mce_provider_parent_class = NULL;
static ContextProviderProviderIface* context_kit_mce_provider_context_provider_provider_parent_iface = NULL;
static void context_kit_mce_provider_finalize (GObject* obj);
static void _vala_array_free (gpointer array, gint array_length, GDestroyNotify destroy_func);
static int _vala_strcmp0 (const char * str1, const char * str2);

static const char* CONTEXT_KIT_MCE_PROVIDER_keys[] = {CONTEXT_KIT_MCE_PROVIDER_key_edge_up, CONTEXT_KIT_MCE_PROVIDER_key_facing_up, CONTEXT_KIT_MCE_PROVIDER_key_display_state, CONTEXT_KIT_MCE_PROVIDER_key_profile_name, CONTEXT_KIT_MCE_PROVIDER_key_is_flight_mode, CONTEXT_KIT_MCE_PROVIDER_key_is_emergency_mode, CONTEXT_KIT_MCE_PROVIDER_key_in_active_use};

static void g_cclosure_user_marshal_VOID__STRING_STRING_STRING_INT_INT_INT (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);
static void g_cclosure_user_marshal_VOID__STRING_STRING (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);

void context_kit_mce_device_orientation_copy (const ContextKitMCEDeviceOrientation* self, ContextKitMCEDeviceOrientation* dest) {
	const char* _tmp2;
	const char* _tmp1;
	const char* _tmp0;
	_tmp2 = NULL;
	_tmp1 = NULL;
	_tmp0 = NULL;
	dest->rotation = (_tmp0 = self->rotation, (_tmp0 == NULL) ? NULL : g_strdup (_tmp0));
	dest->stand = (_tmp1 = self->stand, (_tmp1 == NULL) ? NULL : g_strdup (_tmp1));
	dest->facing = (_tmp2 = self->facing, (_tmp2 == NULL) ? NULL : g_strdup (_tmp2));
	dest->x = self->x;
	dest->y = self->y;
	dest->z = self->z;
}


void context_kit_mce_device_orientation_destroy (ContextKitMCEDeviceOrientation* self) {
	self->rotation = (g_free (self->rotation), NULL);
	self->stand = (g_free (self->stand), NULL);
	self->facing = (g_free (self->facing), NULL);
}


ContextKitMCEDeviceOrientation* context_kit_mce_device_orientation_dup (const ContextKitMCEDeviceOrientation* self) {
	ContextKitMCEDeviceOrientation* dup;
	dup = g_new0 (ContextKitMCEDeviceOrientation, 1);
	context_kit_mce_device_orientation_copy (self, dup);
	return dup;
}


void context_kit_mce_device_orientation_free (ContextKitMCEDeviceOrientation* self) {
	context_kit_mce_device_orientation_destroy (self);
	g_free (self);
}


GType context_kit_mce_device_orientation_get_type (void) {
	static GType context_kit_mce_device_orientation_type_id = 0;
	if (context_kit_mce_device_orientation_type_id == 0) {
		context_kit_mce_device_orientation_type_id = g_boxed_type_register_static ("ContextKitMCEDeviceOrientation", (GBoxedCopyFunc) context_kit_mce_device_orientation_dup, (GBoxedFreeFunc) context_kit_mce_device_orientation_free);
	}
	return context_kit_mce_device_orientation_type_id;
}


static void context_kit_mce_provider_error_for_subset (ContextKitMCEProvider* self, ContextProviderStringSet* keys, GHashTable* ret, GeeArrayList* unavailable_keys, ContextProviderStringSet* intersect_with) {
	ContextProviderStringSet* intersection;
	g_return_if_fail (self != NULL);
	g_return_if_fail (keys != NULL);
	g_return_if_fail (ret != NULL);
	g_return_if_fail (unavailable_keys != NULL);
	g_return_if_fail (intersect_with != NULL);
	intersection = context_provider_string_set_new_intersection (keys, intersect_with);
	{
		GeeArrayList* _tmp0;
		GeeIterator* _tmp1;
		GeeIterator* _key_it;
		_tmp0 = NULL;
		_tmp1 = NULL;
		_key_it = (_tmp1 = gee_iterable_iterator ((GeeIterable*) (_tmp0 = context_provider_string_set_to_array (intersection))), (_tmp0 == NULL) ? NULL : (_tmp0 = (g_object_unref (_tmp0), NULL)), _tmp1);
		while (gee_iterator_next (_key_it)) {
			char* key;
			GValue _tmp2 = {0};
			GValue nonsense;
			key = (char*) gee_iterator_get (_key_it);
			nonsense = (g_value_init (&_tmp2, G_TYPE_BOOLEAN), _tmp2);
			g_value_set_boolean (&nonsense, FALSE);
			gee_collection_add ((GeeCollection*) unavailable_keys, key);
			key = (g_free (key), NULL);
			G_IS_VALUE (&nonsense) ? (g_value_unset (&nonsense), NULL) : NULL;
		}
		(_key_it == NULL) ? NULL : (_key_it = (g_object_unref (_key_it), NULL));
	}
	(intersection == NULL) ? NULL : (intersection = (context_provider_string_set_unref (intersection), NULL));
}


static gint context_kit_mce_provider_abs (ContextKitMCEProvider* self, gint n) {
	g_return_val_if_fail (self != NULL, 0);
	if (n > 0) {
		return n;
	}
	return -n;
}


static gint context_kit_mce_provider_calculate_orientation (ContextKitMCEProvider* self, gint x, gint y) {
	g_return_val_if_fail (self != NULL, 0);
	if (context_kit_mce_provider_abs (self, x) > context_kit_mce_provider_abs (self, y)) {
		if (x > 0) {
			/* Right side up*/
			return 3;
		} else {
			/* Left side up*/
			return 2;
		}
	} else {
		if (context_kit_mce_provider_abs (self, y) > context_kit_mce_provider_abs (self, x)) {
			if (y > 0) {
				/* Bottom side up*/
				return 4;
			} else {
				/* Top side up*/
				return 1;
			}
		}
	}
	/* Undefined*/
	return 0;
}


void _dynamic_get_device_orientation1 (DBusGProxy* self, char** param1, char** param2, char** param3, gint* param4, gint* param5, gint* param6, GError** error) {
	dbus_g_proxy_call (self, "get_device_orientation", error, G_TYPE_INVALID, G_TYPE_STRING, param1, G_TYPE_STRING, param2, G_TYPE_STRING, param3, G_TYPE_INT, param4, G_TYPE_INT, param5, G_TYPE_INT, param6, G_TYPE_INVALID);
	if (*error) {
		return;
	}
}


/*
Fetches orientation information from MCE.
*/
static void context_kit_mce_provider_get_orientation (ContextKitMCEProvider* self, ContextProviderStringSet* keys, GHashTable* ret, GeeArrayList* unavail) {
	GError * inner_error;
	ContextKitMCEDeviceOrientation _tmp0 = {0};
	ContextKitMCEDeviceOrientation orientation;
	g_return_if_fail (self != NULL);
	g_return_if_fail (keys != NULL);
	g_return_if_fail (ret != NULL);
	g_return_if_fail (unavail != NULL);
	inner_error = NULL;
	if (context_provider_string_set_is_disjoint (keys, self->priv->orientation_keys)) {
		return;
	}
	orientation = (memset (&_tmp0, 0, sizeof (ContextKitMCEDeviceOrientation)), _tmp0);
	{
		char* _tmp6;
		char* _tmp5;
		char* _tmp4;
		char* _tmp3;
		char* _tmp2;
		char* _tmp1;
		_tmp6 = NULL;
		_tmp5 = NULL;
		_tmp4 = NULL;
		_tmp3 = NULL;
		_tmp2 = NULL;
		_tmp1 = NULL;
		((_dynamic_get_device_orientation1 (self->priv->mce_request, &_tmp1, &_tmp3, &_tmp5, &orientation.x, &orientation.y, &orientation.z, &inner_error), orientation.rotation = (_tmp2 = _tmp1, orientation.rotation = (g_free (orientation.rotation), NULL), _tmp2)), orientation.stand = (_tmp4 = _tmp3, orientation.stand = (g_free (orientation.stand), NULL), _tmp4));
		orientation.facing = (_tmp6 = _tmp5, orientation.facing = (g_free (orientation.facing), NULL), _tmp6);
		if (inner_error != NULL) {
			goto __catch1_g_error;
			goto __finally1;
		}
	}
	goto __finally1;
	__catch1_g_error:
	{
		GError * ex;
		ex = inner_error;
		inner_error = NULL;
		{
			fprintf (stdout, "MCE Provider Error: %s\n", ex->message);
			context_kit_mce_provider_error_for_subset (self, keys, ret, unavail, self->priv->orientation_keys);
			(ex == NULL) ? NULL : (ex = (g_error_free (ex), NULL));
			context_kit_mce_device_orientation_destroy (&orientation);
			return;
		}
	}
	__finally1:
	if (inner_error != NULL) {
		context_kit_mce_device_orientation_destroy (&orientation);
		g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, inner_error->message);
		g_clear_error (&inner_error);
		return;
	}
	g_debug ("MCE.vala:113: Got orientation %s %s %s (%d,%d,%d)", orientation.rotation, orientation.stand, orientation.facing, orientation.x, orientation.y, orientation.z);
	context_kit_mce_provider_insert_orientation_to_map (self, keys, &orientation, ret, unavail);
	context_kit_mce_device_orientation_destroy (&orientation);
}


void _dynamic_get_display_status2 (DBusGProxy* self, char** param1, GError** error) {
	dbus_g_proxy_call (self, "get_display_status", error, G_TYPE_INVALID, G_TYPE_STRING, param1, G_TYPE_INVALID);
	if (*error) {
		return;
	}
}


/*
Fetches display status information from MCE.
*/
static void context_kit_mce_provider_get_display_status (ContextKitMCEProvider* self, ContextProviderStringSet* keys, GHashTable* ret, GeeArrayList* unavail) {
	GError * inner_error;
	char* display_status;
	g_return_if_fail (self != NULL);
	g_return_if_fail (keys != NULL);
	g_return_if_fail (ret != NULL);
	g_return_if_fail (unavail != NULL);
	inner_error = NULL;
	if (context_provider_string_set_is_disjoint (keys, self->priv->display_status_keys)) {
		return;
	}
	display_status = NULL;
	{
		char* _tmp1;
		char* _tmp0;
		_tmp1 = NULL;
		_tmp0 = NULL;
		_dynamic_get_display_status2 (self->priv->mce_request, &_tmp0, &inner_error);
		display_status = (_tmp1 = _tmp0, display_status = (g_free (display_status), NULL), _tmp1);
		if (inner_error != NULL) {
			goto __catch2_g_error;
			goto __finally2;
		}
	}
	goto __finally2;
	__catch2_g_error:
	{
		GError * ex;
		ex = inner_error;
		inner_error = NULL;
		{
			fprintf (stdout, "MCE Provider Error: %s\n", ex->message);
			context_kit_mce_provider_error_for_subset (self, keys, ret, unavail, self->priv->display_status_keys);
			(ex == NULL) ? NULL : (ex = (g_error_free (ex), NULL));
			display_status = (g_free (display_status), NULL);
			return;
		}
	}
	__finally2:
	if (inner_error != NULL) {
		display_status = (g_free (display_status), NULL);
		g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, inner_error->message);
		g_clear_error (&inner_error);
		return;
	}
	g_debug ("MCE.vala:137: Got display status %s", display_status);
	context_kit_mce_provider_insert_display_status_to_map (self, keys, display_status, ret, unavail);
	display_status = (g_free (display_status), NULL);
}


void _dynamic_get_device_mode3 (DBusGProxy* self, char** param1, GError** error) {
	dbus_g_proxy_call (self, "get_device_mode", error, G_TYPE_INVALID, G_TYPE_STRING, param1, G_TYPE_INVALID);
	if (*error) {
		return;
	}
}


/*
Fetches device mode information from MCE.
*/
static void context_kit_mce_provider_get_device_mode (ContextKitMCEProvider* self, ContextProviderStringSet* keys, GHashTable* ret, GeeArrayList* unavail) {
	GError * inner_error;
	char* device_mode;
	g_return_if_fail (self != NULL);
	g_return_if_fail (keys != NULL);
	g_return_if_fail (ret != NULL);
	g_return_if_fail (unavail != NULL);
	inner_error = NULL;
	if (context_provider_string_set_is_disjoint (keys, self->priv->device_mode_keys)) {
		return;
	}
	device_mode = NULL;
	{
		char* _tmp1;
		char* _tmp0;
		_tmp1 = NULL;
		_tmp0 = NULL;
		_dynamic_get_device_mode3 (self->priv->mce_request, &_tmp0, &inner_error);
		device_mode = (_tmp1 = _tmp0, device_mode = (g_free (device_mode), NULL), _tmp1);
		if (inner_error != NULL) {
			goto __catch3_g_error;
			goto __finally3;
		}
	}
	goto __finally3;
	__catch3_g_error:
	{
		GError * ex;
		ex = inner_error;
		inner_error = NULL;
		{
			fprintf (stdout, "MCE Provider Error: %s\n", ex->message);
			context_kit_mce_provider_error_for_subset (self, keys, ret, unavail, self->priv->device_mode_keys);
			(ex == NULL) ? NULL : (ex = (g_error_free (ex), NULL));
			device_mode = (g_free (device_mode), NULL);
			return;
		}
	}
	__finally3:
	if (inner_error != NULL) {
		device_mode = (g_free (device_mode), NULL);
		g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, inner_error->message);
		g_clear_error (&inner_error);
		return;
	}
	g_debug ("MCE.vala:161: Got device mode %s", device_mode);
	context_kit_mce_provider_insert_device_mode_to_map (self, keys, device_mode, ret, unavail);
	device_mode = (g_free (device_mode), NULL);
}


void _dynamic_get_call_state4 (DBusGProxy* self, char** param1, char** param2, GError** error) {
	dbus_g_proxy_call (self, "get_call_state", error, G_TYPE_INVALID, G_TYPE_STRING, param1, G_TYPE_STRING, param2, G_TYPE_INVALID);
	if (*error) {
		return;
	}
}


/*
Fetches call state information from MCE.
*/
static void context_kit_mce_provider_get_call_state (ContextKitMCEProvider* self, ContextProviderStringSet* keys, GHashTable* ret, GeeArrayList* unavail) {
	GError * inner_error;
	char* state;
	char* type;
	g_return_if_fail (self != NULL);
	g_return_if_fail (keys != NULL);
	g_return_if_fail (ret != NULL);
	g_return_if_fail (unavail != NULL);
	inner_error = NULL;
	if (context_provider_string_set_is_disjoint (keys, self->priv->call_state_keys)) {
		return;
	}
	state = NULL;
	type = NULL;
	{
		char* _tmp3;
		char* _tmp2;
		char* _tmp1;
		char* _tmp0;
		_tmp3 = NULL;
		_tmp2 = NULL;
		_tmp1 = NULL;
		_tmp0 = NULL;
		(_dynamic_get_call_state4 (self->priv->mce_request, &_tmp0, &_tmp2, &inner_error), state = (_tmp1 = _tmp0, state = (g_free (state), NULL), _tmp1));
		type = (_tmp3 = _tmp2, type = (g_free (type), NULL), _tmp3);
		if (inner_error != NULL) {
			goto __catch4_g_error;
			goto __finally4;
		}
	}
	goto __finally4;
	__catch4_g_error:
	{
		GError * ex;
		ex = inner_error;
		inner_error = NULL;
		{
			fprintf (stdout, "MCE Provider Error: %s\n", ex->message);
			context_kit_mce_provider_error_for_subset (self, keys, ret, unavail, self->priv->call_state_keys);
			(ex == NULL) ? NULL : (ex = (g_error_free (ex), NULL));
			state = (g_free (state), NULL);
			type = (g_free (type), NULL);
			return;
		}
	}
	__finally4:
	if (inner_error != NULL) {
		state = (g_free (state), NULL);
		type = (g_free (type), NULL);
		g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, inner_error->message);
		g_clear_error (&inner_error);
		return;
	}
	g_debug ("MCE.vala:185: Got call state %s %s", state, type);
	context_kit_mce_provider_insert_call_state_to_map (self, keys, state, type, ret, unavail);
	state = (g_free (state), NULL);
	type = (g_free (type), NULL);
}


void _dynamic_get_inactivity_status5 (DBusGProxy* self, gboolean* param1, GError** error) {
	dbus_g_proxy_call (self, "get_inactivity_status", error, G_TYPE_INVALID, G_TYPE_BOOLEAN, param1, G_TYPE_INVALID);
	if (*error) {
		return;
	}
}


/*
Fetches inactivity status information from MCE.
*/
static void context_kit_mce_provider_get_inactivity_status (ContextKitMCEProvider* self, ContextProviderStringSet* keys, GHashTable* ret, GeeArrayList* unavail) {
	GError * inner_error;
	gboolean status;
	const char* _tmp0;
	g_return_if_fail (self != NULL);
	g_return_if_fail (keys != NULL);
	g_return_if_fail (ret != NULL);
	g_return_if_fail (unavail != NULL);
	inner_error = NULL;
	if (context_provider_string_set_is_disjoint (keys, self->priv->inactivity_status_keys)) {
		return;
	}
	status = FALSE;
	{
		_dynamic_get_inactivity_status5 (self->priv->mce_request, &status, &inner_error);
		if (inner_error != NULL) {
			goto __catch5_g_error;
			goto __finally5;
		}
	}
	goto __finally5;
	__catch5_g_error:
	{
		GError * ex;
		ex = inner_error;
		inner_error = NULL;
		{
			fprintf (stdout, "MCE Provider Error: %s\n", ex->message);
			context_kit_mce_provider_error_for_subset (self, keys, ret, unavail, self->priv->call_state_keys);
			(ex == NULL) ? NULL : (ex = (g_error_free (ex), NULL));
			return;
		}
	}
	__finally5:
	if (inner_error != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, inner_error->message);
		g_clear_error (&inner_error);
		return;
	}
	_tmp0 = NULL;
	if (status) {
		_tmp0 = "true";
	} else {
		_tmp0 = "false";
	}
	g_debug ("MCE.vala:208: Got inactivity status %s", (_tmp0));
	context_kit_mce_provider_insert_inactivity_status_to_map (self, keys, status, ret, unavail);
}


static GValue* _g_value_dup (GValue* self) {
	return g_boxed_copy (G_TYPE_VALUE, self);
}


/*
Inserts the orientation properties to the map according to the data given. Used by both Get and Subscribe.
*/
static void context_kit_mce_provider_insert_orientation_to_map (ContextKitMCEProvider* self, ContextProviderStringSet* keys, const ContextKitMCEDeviceOrientation* orientation, GHashTable* ret, GeeArrayList* unavail) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (keys != NULL);
	g_return_if_fail (ret != NULL);
	g_return_if_fail (unavail != NULL);
	if (context_provider_string_set_is_member (keys, CONTEXT_KIT_MCE_PROVIDER_key_edge_up)) {
		GValue _tmp0 = {0};
		GValue v;
		GValue* _tmp1 = {0};
		v = (g_value_init (&_tmp0, G_TYPE_INT), _tmp0);
		g_value_set_int (&v, context_kit_mce_provider_calculate_orientation (self, (*orientation).x, (*orientation).y));
		g_hash_table_insert (ret, g_strdup (CONTEXT_KIT_MCE_PROVIDER_key_edge_up), (_tmp1 = &v, (_tmp1 == NULL) ? NULL : _g_value_dup (_tmp1)));
		G_IS_VALUE (&v) ? (g_value_unset (&v), NULL) : NULL;
	}
	if (context_provider_string_set_is_member (keys, CONTEXT_KIT_MCE_PROVIDER_key_facing_up)) {
		GValue _tmp2 = {0};
		GValue v;
		GValue* _tmp3 = {0};
		v = (g_value_init (&_tmp2, G_TYPE_INT), _tmp2);
		if (_vala_strcmp0 ((*orientation).facing, "face_up") == 0) {
			g_value_set_int (&v, 1);
		} else {
			if (_vala_strcmp0 ((*orientation).facing, "face_down") == 0) {
				g_value_set_int (&v, 2);
			} else {
				g_value_set_int (&v, 0);
			}
		}
		g_hash_table_insert (ret, g_strdup (CONTEXT_KIT_MCE_PROVIDER_key_facing_up), (_tmp3 = &v, (_tmp3 == NULL) ? NULL : _g_value_dup (_tmp3)));
		G_IS_VALUE (&v) ? (g_value_unset (&v), NULL) : NULL;
	}
}


/*
Inserts the display state properties to the map according to the data given. Used by both Get and Subscribe.
*/
static void context_kit_mce_provider_insert_display_status_to_map (ContextKitMCEProvider* self, ContextProviderStringSet* keys, const char* display_status, GHashTable* ret, GeeArrayList* unavail) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (keys != NULL);
	g_return_if_fail (display_status != NULL);
	g_return_if_fail (ret != NULL);
	g_return_if_fail (unavail != NULL);
	if (context_provider_string_set_is_member (keys, CONTEXT_KIT_MCE_PROVIDER_key_display_state)) {
		GValue _tmp0 = {0};
		GValue v;
		GValue* _tmp1 = {0};
		v = (g_value_init (&_tmp0, G_TYPE_INT), _tmp0);
		if (_vala_strcmp0 (display_status, "off") == 0) {
			g_value_set_int (&v, 0);
		} else {
			if (_vala_strcmp0 (display_status, "on") == 0) {
				g_value_set_int (&v, 1);
			} else {
				if (_vala_strcmp0 (display_status, "dimmed") == 0) {
					g_value_set_int (&v, 2);
				} else {
					fprintf (stdout, "MCE plugin: Unknown display status %s\n", display_status);
					gee_collection_add ((GeeCollection*) unavail, CONTEXT_KIT_MCE_PROVIDER_key_display_state);
					G_IS_VALUE (&v) ? (g_value_unset (&v), NULL) : NULL;
					return;
				}
			}
		}
		g_hash_table_insert (ret, g_strdup (CONTEXT_KIT_MCE_PROVIDER_key_display_state), (_tmp1 = &v, (_tmp1 == NULL) ? NULL : _g_value_dup (_tmp1)));
		G_IS_VALUE (&v) ? (g_value_unset (&v), NULL) : NULL;
	}
}


/*
Inserts the device mode properties to the map according to the data given. Used by both Get and Subscribe.
*/
static void context_kit_mce_provider_insert_device_mode_to_map (ContextKitMCEProvider* self, ContextProviderStringSet* keys, const char* device_mode, GHashTable* ret, GeeArrayList* unavail) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (keys != NULL);
	g_return_if_fail (device_mode != NULL);
	g_return_if_fail (ret != NULL);
	g_return_if_fail (unavail != NULL);
	if (context_provider_string_set_is_member (keys, CONTEXT_KIT_MCE_PROVIDER_key_is_flight_mode)) {
		GValue _tmp0 = {0};
		GValue v;
		GValue* _tmp1 = {0};
		v = (g_value_init (&_tmp0, G_TYPE_BOOLEAN), _tmp0);
		if (_vala_strcmp0 (device_mode, "normal") == 0) {
			g_value_set_boolean (&v, FALSE);
		} else {
			if (_vala_strcmp0 (device_mode, "flight") == 0) {
				g_value_set_boolean (&v, TRUE);
			} else {
				fprintf (stdout, "MCE plugin: Unknown device mode %s\n", device_mode);
				gee_collection_add ((GeeCollection*) unavail, CONTEXT_KIT_MCE_PROVIDER_key_is_flight_mode);
				G_IS_VALUE (&v) ? (g_value_unset (&v), NULL) : NULL;
				return;
			}
		}
		g_hash_table_insert (ret, g_strdup (CONTEXT_KIT_MCE_PROVIDER_key_is_flight_mode), (_tmp1 = &v, (_tmp1 == NULL) ? NULL : _g_value_dup (_tmp1)));
		G_IS_VALUE (&v) ? (g_value_unset (&v), NULL) : NULL;
	}
}


/*
Inserts the call state properties to the map according to the data given. Used by both Get and Subscribe.
*/
static void context_kit_mce_provider_insert_call_state_to_map (ContextKitMCEProvider* self, ContextProviderStringSet* keys, const char* state, const char* type, GHashTable* ret, GeeArrayList* unavail) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (keys != NULL);
	g_return_if_fail (state != NULL);
	g_return_if_fail (type != NULL);
	g_return_if_fail (ret != NULL);
	g_return_if_fail (unavail != NULL);
	if (context_provider_string_set_is_member (keys, CONTEXT_KIT_MCE_PROVIDER_key_is_emergency_mode)) {
		GValue _tmp0 = {0};
		GValue v;
		GValue* _tmp3 = {0};
		v = (g_value_init (&_tmp0, G_TYPE_BOOLEAN), _tmp0);
		if (_vala_strcmp0 (state, "none") == 0) {
			g_value_set_boolean (&v, FALSE);
		} else {
			gboolean _tmp1;
			gboolean _tmp2;
			_tmp1 = FALSE;
			_tmp2 = FALSE;
			if (_vala_strcmp0 (state, "cellular") == 0) {
				_tmp2 = TRUE;
			} else {
				_tmp2 = _vala_strcmp0 (state, "voip") == 0;
			}
			if (_tmp2) {
				_tmp1 = TRUE;
			} else {
				_tmp1 = _vala_strcmp0 (state, "video") == 0;
			}
			if (_tmp1) {
				if (_vala_strcmp0 (type, "normal") == 0) {
					g_value_set_boolean (&v, FALSE);
				} else {
					if (_vala_strcmp0 (type, "emergency") == 0) {
						g_value_set_boolean (&v, TRUE);
					} else {
						fprintf (stdout, "MCE plugin: Unknown call type %s\n", type);
						gee_collection_add ((GeeCollection*) unavail, CONTEXT_KIT_MCE_PROVIDER_key_is_emergency_mode);
						G_IS_VALUE (&v) ? (g_value_unset (&v), NULL) : NULL;
						return;
					}
				}
			} else {
				fprintf (stdout, "MCE plugin: Unknown call state %s\n", state);
				gee_collection_add ((GeeCollection*) unavail, CONTEXT_KIT_MCE_PROVIDER_key_is_emergency_mode);
				G_IS_VALUE (&v) ? (g_value_unset (&v), NULL) : NULL;
				return;
			}
		}
		g_hash_table_insert (ret, g_strdup (CONTEXT_KIT_MCE_PROVIDER_key_is_emergency_mode), (_tmp3 = &v, (_tmp3 == NULL) ? NULL : _g_value_dup (_tmp3)));
		G_IS_VALUE (&v) ? (g_value_unset (&v), NULL) : NULL;
	}
}


/*
Inserts the inactivity status properties to the map according to the data given. Used by both Get and Subscribe.
*/
static void context_kit_mce_provider_insert_inactivity_status_to_map (ContextKitMCEProvider* self, ContextProviderStringSet* keys, gboolean status, GHashTable* ret, GeeArrayList* unavail) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (keys != NULL);
	g_return_if_fail (ret != NULL);
	g_return_if_fail (unavail != NULL);
	if (context_provider_string_set_is_member (keys, CONTEXT_KIT_MCE_PROVIDER_key_in_active_use)) {
		GValue _tmp0 = {0};
		GValue v;
		GValue* _tmp1 = {0};
		v = (g_value_init (&_tmp0, G_TYPE_BOOLEAN), _tmp0);
		g_value_set_boolean (&v, !status);
		/* Note the negation*/
		g_hash_table_insert (ret, g_strdup (CONTEXT_KIT_MCE_PROVIDER_key_in_active_use), (_tmp1 = &v, (_tmp1 == NULL) ? NULL : _g_value_dup (_tmp1)));
		G_IS_VALUE (&v) ? (g_value_unset (&v), NULL) : NULL;
	}
}


/*
Is called when a sig_device_orientation_ind signal is received from MCE.
*/
static void context_kit_mce_provider_orientation_changed (ContextKitMCEProvider* self, DBusGProxy* sender, const char* rotation, const char* stand, const char* facing, gint32 x, gint32 y, gint32 z) {
	const char* _tmp2;
	const char* _tmp1;
	const char* _tmp0;
	ContextKitMCEDeviceOrientation _tmp3 = {0};
	ContextKitMCEDeviceOrientation orientation;
	GHashTable* ret;
	GeeArrayList* unavail;
	ContextProviderManager* _tmp4;
	g_return_if_fail (self != NULL);
	g_return_if_fail (sender != NULL);
	g_return_if_fail (rotation != NULL);
	g_return_if_fail (stand != NULL);
	g_return_if_fail (facing != NULL);
	g_debug ("MCE.vala:337: orientation_changed: %s, %s, %s", rotation, stand, facing);
	_tmp2 = NULL;
	_tmp1 = NULL;
	_tmp0 = NULL;
	orientation = (memset (&_tmp3, 0, sizeof (ContextKitMCEDeviceOrientation)), _tmp3.rotation = (_tmp0 = rotation, (_tmp0 == NULL) ? NULL : g_strdup (_tmp0)), _tmp3.stand = (_tmp1 = stand, (_tmp1 == NULL) ? NULL : g_strdup (_tmp1)), _tmp3.facing = (_tmp2 = facing, (_tmp2 == NULL) ? NULL : g_strdup (_tmp2)), _tmp3.x = (gint) x, _tmp3.y = (gint) y, _tmp3.z = (gint) z, _tmp3);
	ret = g_hash_table_new (g_str_hash, g_str_equal);
	unavail = gee_array_list_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, g_free, g_direct_equal);
	context_kit_mce_provider_insert_orientation_to_map (self, self->priv->orientation_keys, &orientation, ret, unavail);
	/* Update the central value table with the new property values*/
	_tmp4 = NULL;
	context_provider_manager_property_values_changed (_tmp4 = context_provider_manager_get_instance (), ret, unavail);
	(_tmp4 == NULL) ? NULL : (_tmp4 = (g_object_unref (_tmp4), NULL));
	context_kit_mce_device_orientation_destroy (&orientation);
	(ret == NULL) ? NULL : (ret = (g_hash_table_unref (ret), NULL));
	(unavail == NULL) ? NULL : (unavail = (g_object_unref (unavail), NULL));
}


/* FIXME: Is this how it should be done? 

Is called when a display_status_ind signal is received from MCE.
*/
static void context_kit_mce_provider_display_status_changed (ContextKitMCEProvider* self, DBusGProxy* sender, const char* display_status) {
	GHashTable* ret;
	GeeArrayList* unavail;
	ContextProviderManager* _tmp0;
	g_return_if_fail (self != NULL);
	g_return_if_fail (sender != NULL);
	g_return_if_fail (display_status != NULL);
	g_debug ("MCE.vala:354: MCE plugin: Display status changed: %s", display_status);
	ret = g_hash_table_new (g_str_hash, g_str_equal);
	unavail = gee_array_list_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, g_free, g_direct_equal);
	context_kit_mce_provider_insert_display_status_to_map (self, self->priv->display_status_keys, display_status, ret, unavail);
	/* Update the central value table with the new property values*/
	_tmp0 = NULL;
	context_provider_manager_property_values_changed (_tmp0 = context_provider_manager_get_instance (), ret, unavail);
	(_tmp0 == NULL) ? NULL : (_tmp0 = (g_object_unref (_tmp0), NULL));
	(ret == NULL) ? NULL : (ret = (g_hash_table_unref (ret), NULL));
	(unavail == NULL) ? NULL : (unavail = (g_object_unref (unavail), NULL));
}


/*
Is called when a sig_device_mode_ind signal is received from MCE.
*/
static void context_kit_mce_provider_device_mode_changed (ContextKitMCEProvider* self, DBusGProxy* sender, const char* device_mode) {
	GHashTable* ret;
	GeeArrayList* unavail;
	ContextProviderManager* _tmp0;
	g_return_if_fail (self != NULL);
	g_return_if_fail (sender != NULL);
	g_return_if_fail (device_mode != NULL);
	g_debug ("MCE.vala:369: MCE plugin: Device mode changed: %s", device_mode);
	ret = g_hash_table_new (g_str_hash, g_str_equal);
	unavail = gee_array_list_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, g_free, g_direct_equal);
	context_kit_mce_provider_insert_device_mode_to_map (self, self->priv->device_mode_keys, device_mode, ret, unavail);
	/* Update the central value table with the new property values*/
	_tmp0 = NULL;
	context_provider_manager_property_values_changed (_tmp0 = context_provider_manager_get_instance (), ret, unavail);
	(_tmp0 == NULL) ? NULL : (_tmp0 = (g_object_unref (_tmp0), NULL));
	(ret == NULL) ? NULL : (ret = (g_hash_table_unref (ret), NULL));
	(unavail == NULL) ? NULL : (unavail = (g_object_unref (unavail), NULL));
}


/*
Is called when a sig_call_state_ind signal is received from MCE.
*/
static void context_kit_mce_provider_call_state_changed (ContextKitMCEProvider* self, DBusGProxy* sender, const char* state, const char* type) {
	GHashTable* ret;
	GeeArrayList* unavail;
	ContextProviderManager* _tmp0;
	g_return_if_fail (self != NULL);
	g_return_if_fail (sender != NULL);
	g_return_if_fail (state != NULL);
	g_return_if_fail (type != NULL);
	g_debug ("MCE.vala:384: MCE plugin: Call state changed: %s %s", state, type);
	ret = g_hash_table_new (g_str_hash, g_str_equal);
	unavail = gee_array_list_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, g_free, g_direct_equal);
	context_kit_mce_provider_insert_call_state_to_map (self, self->priv->call_state_keys, state, type, ret, unavail);
	/* Update the central value table with the new property values*/
	_tmp0 = NULL;
	context_provider_manager_property_values_changed (_tmp0 = context_provider_manager_get_instance (), ret, unavail);
	(_tmp0 == NULL) ? NULL : (_tmp0 = (g_object_unref (_tmp0), NULL));
	(ret == NULL) ? NULL : (ret = (g_hash_table_unref (ret), NULL));
	(unavail == NULL) ? NULL : (unavail = (g_object_unref (unavail), NULL));
}


/*
Is called when a system_call_state_ind signal is received from MCE.
*/
static void context_kit_mce_provider_inactivity_status_changed (ContextKitMCEProvider* self, DBusGProxy* sender, gboolean status) {
	const char* _tmp0;
	GHashTable* ret;
	GeeArrayList* unavail;
	ContextProviderManager* _tmp1;
	g_return_if_fail (self != NULL);
	g_return_if_fail (sender != NULL);
	_tmp0 = NULL;
	if (status) {
		_tmp0 = "true";
	} else {
		_tmp0 = "false";
	}
	g_debug ("MCE.vala:399: MCE plugin: Inactivity state changed: %s", (_tmp0));
	ret = g_hash_table_new (g_str_hash, g_str_equal);
	unavail = gee_array_list_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, g_free, g_direct_equal);
	context_kit_mce_provider_insert_inactivity_status_to_map (self, self->priv->inactivity_status_keys, status, ret, unavail);
	/* Update the central value table with the new property values*/
	_tmp1 = NULL;
	context_provider_manager_property_values_changed (_tmp1 = context_provider_manager_get_instance (), ret, unavail);
	(_tmp1 == NULL) ? NULL : (_tmp1 = (g_object_unref (_tmp1), NULL));
	(ret == NULL) ? NULL : (ret = (g_hash_table_unref (ret), NULL));
	(unavail == NULL) ? NULL : (unavail = (g_object_unref (unavail), NULL));
}


ContextKitMCEProvider* context_kit_mce_provider_construct (GType object_type) {
	GError * inner_error;
	ContextKitMCEProvider * self;
	DBusGConnection* _tmp0;
	DBusGConnection* _tmp1;
	DBusGProxy* _tmp2;
	ContextProviderStringSet* _tmp5;
	gint _tmp4_length1;
	char** _tmp4;
	char** _tmp3;
	ContextProviderStringSet* _tmp8;
	gint _tmp7_length1;
	char** _tmp7;
	char** _tmp6;
	ContextProviderStringSet* _tmp11;
	gint _tmp10_length1;
	char** _tmp10;
	char** _tmp9;
	ContextProviderStringSet* _tmp14;
	gint _tmp13_length1;
	char** _tmp13;
	char** _tmp12;
	ContextProviderStringSet* _tmp17;
	gint _tmp16_length1;
	char** _tmp16;
	char** _tmp15;
	inner_error = NULL;
	self = g_object_newv (object_type, 0, NULL);
	_tmp0 = dbus_g_bus_get (DBUS_BUS_SYSTEM, &inner_error);
	if (inner_error != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, inner_error->message);
		g_clear_error (&inner_error);
		return;
	}
	_tmp1 = NULL;
	self->priv->conn = (_tmp1 = _tmp0, (self->priv->conn == NULL) ? NULL : (self->priv->conn = (dbus_g_connection_unref (self->priv->conn), NULL)), _tmp1);
	_tmp2 = NULL;
	self->priv->mce_request = (_tmp2 = dbus_g_proxy_new_for_name (self->priv->conn, "com.nokia.mce", "/com/nokia/mce/request", "com.nokia.mce.request"), (self->priv->mce_request == NULL) ? NULL : (self->priv->mce_request = (g_object_unref (self->priv->mce_request), NULL)), _tmp2);
	_tmp5 = NULL;
	_tmp4 = NULL;
	_tmp3 = NULL;
	self->priv->orientation_keys = (_tmp5 = context_provider_string_set_new_from_array ((_tmp4 = (_tmp3 = g_new0 (char*, 2 + 1), _tmp3[0] = g_strdup (CONTEXT_KIT_MCE_PROVIDER_key_edge_up), _tmp3[1] = g_strdup (CONTEXT_KIT_MCE_PROVIDER_key_facing_up), _tmp3), _tmp4_length1 = 2, _tmp4)), (self->priv->orientation_keys == NULL) ? NULL : (self->priv->orientation_keys = (context_provider_string_set_unref (self->priv->orientation_keys), NULL)), _tmp5);
	_tmp4 = (_vala_array_free (_tmp4, _tmp4_length1, (GDestroyNotify) g_free), NULL);
	_tmp8 = NULL;
	_tmp7 = NULL;
	_tmp6 = NULL;
	self->priv->display_status_keys = (_tmp8 = context_provider_string_set_new_from_array ((_tmp7 = (_tmp6 = g_new0 (char*, 1 + 1), _tmp6[0] = g_strdup (CONTEXT_KIT_MCE_PROVIDER_key_display_state), _tmp6), _tmp7_length1 = 1, _tmp7)), (self->priv->display_status_keys == NULL) ? NULL : (self->priv->display_status_keys = (context_provider_string_set_unref (self->priv->display_status_keys), NULL)), _tmp8);
	_tmp7 = (_vala_array_free (_tmp7, _tmp7_length1, (GDestroyNotify) g_free), NULL);
	_tmp11 = NULL;
	_tmp10 = NULL;
	_tmp9 = NULL;
	self->priv->device_mode_keys = (_tmp11 = context_provider_string_set_new_from_array ((_tmp10 = (_tmp9 = g_new0 (char*, 1 + 1), _tmp9[0] = g_strdup (CONTEXT_KIT_MCE_PROVIDER_key_is_flight_mode), _tmp9), _tmp10_length1 = 1, _tmp10)), (self->priv->device_mode_keys == NULL) ? NULL : (self->priv->device_mode_keys = (context_provider_string_set_unref (self->priv->device_mode_keys), NULL)), _tmp11);
	_tmp10 = (_vala_array_free (_tmp10, _tmp10_length1, (GDestroyNotify) g_free), NULL);
	_tmp14 = NULL;
	_tmp13 = NULL;
	_tmp12 = NULL;
	self->priv->call_state_keys = (_tmp14 = context_provider_string_set_new_from_array ((_tmp13 = (_tmp12 = g_new0 (char*, 1 + 1), _tmp12[0] = g_strdup (CONTEXT_KIT_MCE_PROVIDER_key_is_emergency_mode), _tmp12), _tmp13_length1 = 1, _tmp13)), (self->priv->call_state_keys == NULL) ? NULL : (self->priv->call_state_keys = (context_provider_string_set_unref (self->priv->call_state_keys), NULL)), _tmp14);
	_tmp13 = (_vala_array_free (_tmp13, _tmp13_length1, (GDestroyNotify) g_free), NULL);
	_tmp17 = NULL;
	_tmp16 = NULL;
	_tmp15 = NULL;
	self->priv->inactivity_status_keys = (_tmp17 = context_provider_string_set_new_from_array ((_tmp16 = (_tmp15 = g_new0 (char*, 1 + 1), _tmp15[0] = g_strdup (CONTEXT_KIT_MCE_PROVIDER_key_in_active_use), _tmp15), _tmp16_length1 = 1, _tmp16)), (self->priv->inactivity_status_keys == NULL) ? NULL : (self->priv->inactivity_status_keys = (context_provider_string_set_unref (self->priv->inactivity_status_keys), NULL)), _tmp17);
	_tmp16 = (_vala_array_free (_tmp16, _tmp16_length1, (GDestroyNotify) g_free), NULL);
	return self;
}


ContextKitMCEProvider* context_kit_mce_provider_new (void) {
	return context_kit_mce_provider_construct (CONTEXT_KIT_MCE_TYPE_PROVIDER);
}


static void context_kit_mce_provider_real_get (ContextProviderProvider* base, ContextProviderStringSet* keys, GHashTable* ret, GeeArrayList* unavail) {
	ContextKitMCEProvider * self;
	self = (ContextKitMCEProvider*) base;
	g_return_if_fail (keys != NULL);
	g_return_if_fail (ret != NULL);
	g_return_if_fail (unavail != NULL);
	context_kit_mce_provider_get_orientation (self, keys, ret, unavail);
	context_kit_mce_provider_get_display_status (self, keys, ret, unavail);
	context_kit_mce_provider_get_device_mode (self, keys, ret, unavail);
	context_kit_mce_provider_get_call_state (self, keys, ret, unavail);
	context_kit_mce_provider_get_inactivity_status (self, keys, ret, unavail);
}


static void context_kit_mce_provider_ensure_mce_signal_exists (ContextKitMCEProvider* self) {
	g_return_if_fail (self != NULL);
	if (self->priv->mce_signal == NULL) {
		DBusGProxy* _tmp0;
		_tmp0 = NULL;
		self->priv->mce_signal = (_tmp0 = dbus_g_proxy_new_for_name (self->priv->conn, "com.nokia.mce", "/com/nokia/mce/signal", "com.nokia.mce.signal"), (self->priv->mce_signal == NULL) ? NULL : (self->priv->mce_signal = (g_object_unref (self->priv->mce_signal), NULL)), _tmp0);
	}
}


static void _context_kit_mce_provider_orientation_changed_dynamic_sig_device_orientation_ind0_ (DBusGProxy* _sender, const char* rotation, const char* stand, const char* facing, gint32 x, gint32 y, gint32 z, gpointer self) {
	context_kit_mce_provider_orientation_changed (self, _sender, rotation, stand, facing, x, y, z);
}


void _dynamic_sig_device_orientation_ind1_connect (gpointer obj, const char * signal_name, GCallback handler, gpointer data) {
	dbus_g_object_register_marshaller (g_cclosure_user_marshal_VOID__STRING_STRING_STRING_INT_INT_INT, G_TYPE_NONE, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INVALID);
	dbus_g_proxy_add_signal (obj, "sig_device_orientation_ind", G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (obj, signal_name, handler, data, NULL);
}


static void _context_kit_mce_provider_display_status_changed_dynamic_display_status_ind2_ (DBusGProxy* _sender, const char* display_status, gpointer self) {
	context_kit_mce_provider_display_status_changed (self, _sender, display_status);
}


void _dynamic_display_status_ind3_connect (gpointer obj, const char * signal_name, GCallback handler, gpointer data) {
	dbus_g_object_register_marshaller (g_cclosure_marshal_VOID__STRING, G_TYPE_NONE, G_TYPE_STRING, G_TYPE_INVALID);
	dbus_g_proxy_add_signal (obj, "display_status_ind", G_TYPE_STRING, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (obj, signal_name, handler, data, NULL);
}


static void _context_kit_mce_provider_device_mode_changed_dynamic_sig_device_mode_ind4_ (DBusGProxy* _sender, const char* device_mode, gpointer self) {
	context_kit_mce_provider_device_mode_changed (self, _sender, device_mode);
}


void _dynamic_sig_device_mode_ind5_connect (gpointer obj, const char * signal_name, GCallback handler, gpointer data) {
	dbus_g_object_register_marshaller (g_cclosure_marshal_VOID__STRING, G_TYPE_NONE, G_TYPE_STRING, G_TYPE_INVALID);
	dbus_g_proxy_add_signal (obj, "sig_device_mode_ind", G_TYPE_STRING, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (obj, signal_name, handler, data, NULL);
}


static void _context_kit_mce_provider_call_state_changed_dynamic_sig_call_state_ind6_ (DBusGProxy* _sender, const char* state, const char* type, gpointer self) {
	context_kit_mce_provider_call_state_changed (self, _sender, state, type);
}


void _dynamic_sig_call_state_ind7_connect (gpointer obj, const char * signal_name, GCallback handler, gpointer data) {
	dbus_g_object_register_marshaller (g_cclosure_user_marshal_VOID__STRING_STRING, G_TYPE_NONE, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INVALID);
	dbus_g_proxy_add_signal (obj, "sig_call_state_ind", G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (obj, signal_name, handler, data, NULL);
}


static void _context_kit_mce_provider_inactivity_status_changed_dynamic_system_inactivity_ind8_ (DBusGProxy* _sender, gboolean status, gpointer self) {
	context_kit_mce_provider_inactivity_status_changed (self, _sender, status);
}


void _dynamic_system_inactivity_ind9_connect (gpointer obj, const char * signal_name, GCallback handler, gpointer data) {
	dbus_g_object_register_marshaller (g_cclosure_marshal_VOID__BOOLEAN, G_TYPE_NONE, G_TYPE_BOOLEAN, G_TYPE_INVALID);
	dbus_g_proxy_add_signal (obj, "system_inactivity_ind", G_TYPE_BOOLEAN, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (obj, signal_name, handler, data, NULL);
}


static void context_kit_mce_provider_real_keys_subscribed (ContextProviderProvider* base, ContextProviderStringSet* keys) {
	ContextKitMCEProvider * self;
	char* _tmp0;
	self = (ContextKitMCEProvider*) base;
	g_return_if_fail (keys != NULL);
	_tmp0 = NULL;
	g_debug ("MCE.vala:454: MCE plugin: keys_subscribed %s", _tmp0 = context_provider_string_set_debug (keys));
	_tmp0 = (g_free (_tmp0), NULL);
	context_kit_mce_provider_ensure_mce_signal_exists (self);
	/* Connect the corresponding MCE signal to its handler*/
	if (context_provider_string_set_is_disjoint (keys, self->priv->orientation_keys) == FALSE) {
		/* Note: even thoug orientation_keys has to members,
		 this is OK since connecting the same signal twice does not do any harm*/
		_dynamic_sig_device_orientation_ind1_connect (self->priv->mce_signal, "sig_device_orientation_ind", (GCallback) _context_kit_mce_provider_orientation_changed_dynamic_sig_device_orientation_ind0_, self);
	}
	if (context_provider_string_set_is_disjoint (keys, self->priv->display_status_keys) == FALSE) {
		_dynamic_display_status_ind3_connect (self->priv->mce_signal, "display_status_ind", (GCallback) _context_kit_mce_provider_display_status_changed_dynamic_display_status_ind2_, self);
	}
	if (context_provider_string_set_is_disjoint (keys, self->priv->device_mode_keys) == FALSE) {
		_dynamic_sig_device_mode_ind5_connect (self->priv->mce_signal, "sig_device_mode_ind", (GCallback) _context_kit_mce_provider_device_mode_changed_dynamic_sig_device_mode_ind4_, self);
	}
	if (context_provider_string_set_is_disjoint (keys, self->priv->call_state_keys) == FALSE) {
		_dynamic_sig_call_state_ind7_connect (self->priv->mce_signal, "sig_call_state_ind", (GCallback) _context_kit_mce_provider_call_state_changed_dynamic_sig_call_state_ind6_, self);
	}
	if (context_provider_string_set_is_disjoint (keys, self->priv->inactivity_status_keys) == FALSE) {
		_dynamic_system_inactivity_ind9_connect (self->priv->mce_signal, "system_inactivity_ind", (GCallback) _context_kit_mce_provider_inactivity_status_changed_dynamic_system_inactivity_ind8_, self);
	}
}


static void _context_kit_mce_provider_orientation_changed_dynamic_sig_device_orientation_ind10_ (DBusGProxy* _sender, const char* rotation, const char* stand, const char* facing, gint32 x, gint32 y, gint32 z, gpointer self) {
	context_kit_mce_provider_orientation_changed (self, _sender, rotation, stand, facing, x, y, z);
}


void _dynamic_sig_device_orientation_ind11_disconnect (gpointer obj, const char * signal_name, GCallback handler, gpointer data) {
	dbus_g_proxy_disconnect_signal (obj, signal_name, handler, data);
}


static void _context_kit_mce_provider_display_status_changed_dynamic_display_status_ind12_ (DBusGProxy* _sender, const char* display_status, gpointer self) {
	context_kit_mce_provider_display_status_changed (self, _sender, display_status);
}


void _dynamic_display_status_ind13_disconnect (gpointer obj, const char * signal_name, GCallback handler, gpointer data) {
	dbus_g_proxy_disconnect_signal (obj, signal_name, handler, data);
}


static void _context_kit_mce_provider_device_mode_changed_dynamic_sig_device_mode_ind14_ (DBusGProxy* _sender, const char* device_mode, gpointer self) {
	context_kit_mce_provider_device_mode_changed (self, _sender, device_mode);
}


void _dynamic_sig_device_mode_ind15_disconnect (gpointer obj, const char * signal_name, GCallback handler, gpointer data) {
	dbus_g_proxy_disconnect_signal (obj, signal_name, handler, data);
}


static void _context_kit_mce_provider_call_state_changed_dynamic_sig_call_state_ind16_ (DBusGProxy* _sender, const char* state, const char* type, gpointer self) {
	context_kit_mce_provider_call_state_changed (self, _sender, state, type);
}


void _dynamic_sig_call_state_ind17_disconnect (gpointer obj, const char * signal_name, GCallback handler, gpointer data) {
	dbus_g_proxy_disconnect_signal (obj, signal_name, handler, data);
}


static void _context_kit_mce_provider_inactivity_status_changed_dynamic_system_inactivity_ind18_ (DBusGProxy* _sender, gboolean status, gpointer self) {
	context_kit_mce_provider_inactivity_status_changed (self, _sender, status);
}


void _dynamic_system_inactivity_ind19_disconnect (gpointer obj, const char * signal_name, GCallback handler, gpointer data) {
	dbus_g_proxy_disconnect_signal (obj, signal_name, handler, data);
}


static void context_kit_mce_provider_real_keys_unsubscribed (ContextProviderProvider* base, ContextProviderStringSet* keys, ContextProviderStringSet* keys_remaining) {
	ContextKitMCEProvider * self;
	char* _tmp0;
	self = (ContextKitMCEProvider*) base;
	g_return_if_fail (keys != NULL);
	g_return_if_fail (keys_remaining != NULL);
	_tmp0 = NULL;
	g_debug ("MCE.vala:483: MCE plugin: keys_unsubscribed %s", _tmp0 = context_provider_string_set_debug (keys));
	_tmp0 = (g_free (_tmp0), NULL);
	/* Disconnect the corresponding MCE signal from its handler*/
	if (context_provider_string_set_is_disjoint (keys, self->priv->orientation_keys) == FALSE) {
		gint no_of_subscribers;
		/* Note: orientation_keys contains two keys
		 At least one of the keys was unsubscribed, but the other one may still be subscribed to
		 FIXME: How to implement?*/
		no_of_subscribers = 0;
		{
			ContextProviderStringSetIter* _key_it;
			_key_it = context_provider_string_set_iterator (self->priv->orientation_keys);
			while (context_provider_string_set_iter_next (_key_it)) {
				char* key;
				ContextProviderManager* _tmp1;
				key = context_provider_string_set_iter_get (_key_it);
				_tmp1 = NULL;
				no_of_subscribers = no_of_subscribers + (context_provider_key_usage_counter_number_of_subscribers (context_provider_manager_get_key_counter (_tmp1 = context_provider_manager_get_instance ()), key));
				(_tmp1 == NULL) ? NULL : (_tmp1 = (g_object_unref (_tmp1), NULL));
				key = (g_free (key), NULL);
			}
			(_key_it == NULL) ? NULL : (_key_it = (context_provider_string_set_iter_unref (_key_it), NULL));
		}
		if (no_of_subscribers == 0) {
			_dynamic_sig_device_orientation_ind11_disconnect (self->priv->mce_signal, "sig_device_orientation_ind", (GCallback) _context_kit_mce_provider_orientation_changed_dynamic_sig_device_orientation_ind10_, self);
		}
	}
	/* The other key sets contain only one key, so unsubscription is straightforward*/
	if (context_provider_string_set_is_disjoint (keys, self->priv->display_status_keys) == FALSE) {
		_dynamic_display_status_ind13_disconnect (self->priv->mce_signal, "display_status_ind", (GCallback) _context_kit_mce_provider_display_status_changed_dynamic_display_status_ind12_, self);
	}
	if (context_provider_string_set_is_disjoint (keys, self->priv->device_mode_keys) == FALSE) {
		_dynamic_sig_device_mode_ind15_disconnect (self->priv->mce_signal, "sig_device_mode_ind", (GCallback) _context_kit_mce_provider_device_mode_changed_dynamic_sig_device_mode_ind14_, self);
	}
	if (context_provider_string_set_is_disjoint (keys, self->priv->call_state_keys) == FALSE) {
		_dynamic_sig_call_state_ind17_disconnect (self->priv->mce_signal, "sig_call_state_ind", (GCallback) _context_kit_mce_provider_call_state_changed_dynamic_sig_call_state_ind16_, self);
	}
	if (context_provider_string_set_is_disjoint (keys, self->priv->inactivity_status_keys) == FALSE) {
		_dynamic_system_inactivity_ind19_disconnect (self->priv->mce_signal, "system_inactivity_ind", (GCallback) _context_kit_mce_provider_inactivity_status_changed_dynamic_system_inactivity_ind18_, self);
	}
}


static ContextProviderStringSet* context_kit_mce_provider_real_provided_keys (ContextProviderProvider* base) {
	ContextKitMCEProvider * self;
	ContextProviderStringSet* _tmp0;
	self = (ContextKitMCEProvider*) base;
	_tmp0 = NULL;
	return (_tmp0 = self->priv->all_keys, (_tmp0 == NULL) ? NULL : context_provider_string_set_ref (_tmp0));
}


static void context_kit_mce_provider_class_init (ContextKitMCEProviderClass * klass) {
	context_kit_mce_provider_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (ContextKitMCEProviderPrivate));
	G_OBJECT_CLASS (klass)->finalize = context_kit_mce_provider_finalize;
}


static void context_kit_mce_provider_context_provider_provider_interface_init (ContextProviderProviderIface * iface) {
	context_kit_mce_provider_context_provider_provider_parent_iface = g_type_interface_peek_parent (iface);
	iface->get = context_kit_mce_provider_real_get;
	iface->keys_subscribed = context_kit_mce_provider_real_keys_subscribed;
	iface->keys_unsubscribed = context_kit_mce_provider_real_keys_unsubscribed;
	iface->provided_keys = context_kit_mce_provider_real_provided_keys;
}


static void context_kit_mce_provider_instance_init (ContextKitMCEProvider * self) {
	self->priv = CONTEXT_KIT_MCE_PROVIDER_GET_PRIVATE (self);
	self->priv->all_keys = context_provider_string_set_new_from_array (CONTEXT_KIT_MCE_PROVIDER_keys);
}


static void context_kit_mce_provider_finalize (GObject* obj) {
	ContextKitMCEProvider * self;
	self = CONTEXT_KIT_MCE_PROVIDER (obj);
	(self->priv->conn == NULL) ? NULL : (self->priv->conn = (dbus_g_connection_unref (self->priv->conn), NULL));
	(self->priv->mce_request == NULL) ? NULL : (self->priv->mce_request = (g_object_unref (self->priv->mce_request), NULL));
	(self->priv->mce_signal == NULL) ? NULL : (self->priv->mce_signal = (g_object_unref (self->priv->mce_signal), NULL));
	(self->priv->all_keys == NULL) ? NULL : (self->priv->all_keys = (context_provider_string_set_unref (self->priv->all_keys), NULL));
	(self->priv->orientation_keys == NULL) ? NULL : (self->priv->orientation_keys = (context_provider_string_set_unref (self->priv->orientation_keys), NULL));
	(self->priv->display_status_keys == NULL) ? NULL : (self->priv->display_status_keys = (context_provider_string_set_unref (self->priv->display_status_keys), NULL));
	(self->priv->device_mode_keys == NULL) ? NULL : (self->priv->device_mode_keys = (context_provider_string_set_unref (self->priv->device_mode_keys), NULL));
	(self->priv->call_state_keys == NULL) ? NULL : (self->priv->call_state_keys = (context_provider_string_set_unref (self->priv->call_state_keys), NULL));
	(self->priv->inactivity_status_keys == NULL) ? NULL : (self->priv->inactivity_status_keys = (context_provider_string_set_unref (self->priv->inactivity_status_keys), NULL));
	G_OBJECT_CLASS (context_kit_mce_provider_parent_class)->finalize (obj);
}


GType context_kit_mce_provider_get_type (void) {
	static GType context_kit_mce_provider_type_id = 0;
	if (context_kit_mce_provider_type_id == 0) {
		static const GTypeInfo g_define_type_info = { sizeof (ContextKitMCEProviderClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) context_kit_mce_provider_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ContextKitMCEProvider), 0, (GInstanceInitFunc) context_kit_mce_provider_instance_init, NULL };
		static const GInterfaceInfo context_provider_provider_info = { (GInterfaceInitFunc) context_kit_mce_provider_context_provider_provider_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		context_kit_mce_provider_type_id = g_type_register_static (G_TYPE_OBJECT, "ContextKitMCEProvider", &g_define_type_info, 0);
		g_type_add_interface_static (context_kit_mce_provider_type_id, CONTEXT_PROVIDER_TYPE_PROVIDER, &context_provider_provider_info);
	}
	return context_kit_mce_provider_type_id;
}


static void _vala_array_free (gpointer array, gint array_length, GDestroyNotify destroy_func) {
	if ((array != NULL) && (destroy_func != NULL)) {
		int i;
		for (i = 0; i < array_length; i = i + 1) {
			if (((gpointer*) array)[i] != NULL) {
				destroy_func (((gpointer*) array)[i]);
			}
		}
	}
	g_free (array);
}


static int _vala_strcmp0 (const char * str1, const char * str2) {
	if (str1 == NULL) {
		return -(str1 != str2);
	}
	if (str2 == NULL) {
		return str1 != str2;
	}
	return strcmp (str1, str2);
}



static void g_cclosure_user_marshal_VOID__STRING_STRING_STRING_INT_INT_INT (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data) {
	typedef void (*GMarshalFunc_VOID__STRING_STRING_STRING_INT_INT_INT) (gpointer data1, const char* arg_1, const char* arg_2, const char* arg_3, gint32 arg_4, gint32 arg_5, gint32 arg_6, gpointer data2);
	register GMarshalFunc_VOID__STRING_STRING_STRING_INT_INT_INT callback;
	register GCClosure * cc;
	register gpointer data1, data2;
	cc = (GCClosure *) closure;
	g_return_if_fail (n_param_values == 7);
	if (G_CCLOSURE_SWAP_DATA (closure)) {
		data1 = closure->data;
		data2 = param_values->data[0].v_pointer;
	} else {
		data1 = param_values->data[0].v_pointer;
		data2 = closure->data;
	}
	callback = (GMarshalFunc_VOID__STRING_STRING_STRING_INT_INT_INT) (marshal_data ? marshal_data : cc->callback);
	callback (data1, g_value_get_string (param_values + 1), g_value_get_string (param_values + 2), g_value_get_string (param_values + 3), g_value_get_int (param_values + 4), g_value_get_int (param_values + 5), g_value_get_int (param_values + 6), data2);
}


static void g_cclosure_user_marshal_VOID__STRING_STRING (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data) {
	typedef void (*GMarshalFunc_VOID__STRING_STRING) (gpointer data1, const char* arg_1, const char* arg_2, gpointer data2);
	register GMarshalFunc_VOID__STRING_STRING callback;
	register GCClosure * cc;
	register gpointer data1, data2;
	cc = (GCClosure *) closure;
	g_return_if_fail (n_param_values == 3);
	if (G_CCLOSURE_SWAP_DATA (closure)) {
		data1 = closure->data;
		data2 = param_values->data[0].v_pointer;
	} else {
		data1 = param_values->data[0].v_pointer;
		data2 = closure->data;
	}
	callback = (GMarshalFunc_VOID__STRING_STRING) (marshal_data ? marshal_data : cc->callback);
	callback (data1, g_value_get_string (param_values + 1), g_value_get_string (param_values + 2), data2);
}



