
#include <dbus_interface.h>

typedef DBusGProxy ContextProviderDBusManagerDBusProxy;
typedef DBusGProxyClass ContextProviderDBusManagerDBusProxyClass;
typedef DBusGProxy ContextProviderDBusSubscriberDBusProxy;
typedef DBusGProxyClass ContextProviderDBusSubscriberDBusProxyClass;
typedef struct _DBusObjectVTable _DBusObjectVTable;

struct _DBusObjectVTable {
	void (*register_object) (DBusConnection*, const char*, void*);
};



void _context_provider_dbus_manager_dbus_unregister (DBusConnection* connection, void* user_data);
static DBusMessage* _dbus_context_provider_dbus_manager_introspect (ContextProviderDBusManager* self, DBusConnection* connection, DBusMessage* message);
static DBusMessage* _dbus_context_provider_dbus_manager_property_get_all (ContextProviderDBusManager* self, DBusConnection* connection, DBusMessage* message);
static DBusMessage* _dbus_context_provider_dbus_manager_Get (ContextProviderDBusManager* self, DBusConnection* connection, DBusMessage* message);
static DBusMessage* _dbus_context_provider_dbus_manager_GetSubscriber (ContextProviderDBusManager* self, DBusConnection* connection, DBusMessage* message);
DBusHandlerResult context_provider_dbus_manager_dbus_proxy_filter (DBusConnection* connection, DBusMessage* message, void* user_data);
static void context_provider_dbus_manager_dbus_proxy_Get (ContextProviderDBusManager* self, char** keys, int keys_length1, GHashTable** values, char*** undeterminable_keys, int* undeterminable_keys_length1);
static char* context_provider_dbus_manager_dbus_proxy_GetSubscriber (ContextProviderDBusManager* self, const char* name, GError** error);
static void context_provider_dbus_manager_dbus_proxy_interface_init (ContextProviderDBusManagerIface* iface);
void _context_provider_dbus_subscriber_dbus_unregister (DBusConnection* connection, void* user_data);
static DBusMessage* _dbus_context_provider_dbus_subscriber_introspect (ContextProviderDBusSubscriber* self, DBusConnection* connection, DBusMessage* message);
static DBusMessage* _dbus_context_provider_dbus_subscriber_property_get_all (ContextProviderDBusSubscriber* self, DBusConnection* connection, DBusMessage* message);
static DBusMessage* _dbus_context_provider_dbus_subscriber_Subscribe (ContextProviderDBusSubscriber* self, DBusConnection* connection, DBusMessage* message);
static DBusMessage* _dbus_context_provider_dbus_subscriber_Unsubscribe (ContextProviderDBusSubscriber* self, DBusConnection* connection, DBusMessage* message);
static void _dbus_context_provider_dbus_subscriber_changed (GObject* _sender, GHashTable* values, char** undeterminable_keys, int undeterminable_keys_length1, DBusConnection* _connection);
static void _dbus_handle_context_provider_dbus_subscriber_changed (ContextProviderDBusSubscriber* self, DBusConnection* connection, DBusMessage* message);
DBusHandlerResult context_provider_dbus_subscriber_dbus_proxy_filter (DBusConnection* connection, DBusMessage* message, void* user_data);
static void context_provider_dbus_subscriber_dbus_proxy_Subscribe (ContextProviderDBusSubscriber* self, char** keys, int keys_length1, GHashTable** values, char*** undeterminable_keys, int* undeterminable_keys_length1);
static void context_provider_dbus_subscriber_dbus_proxy_Unsubscribe (ContextProviderDBusSubscriber* self, char** keys, int keys_length1);
static void context_provider_dbus_subscriber_dbus_proxy_interface_init (ContextProviderDBusSubscriberIface* iface);
static void _vala_dbus_register_object (DBusConnection* connection, const char* path, void* object);

static const DBusObjectPathVTable _context_provider_dbus_manager_dbus_path_vtable = {_context_provider_dbus_manager_dbus_unregister, context_provider_dbus_manager_dbus_message};
static const _DBusObjectVTable _context_provider_dbus_manager_dbus_vtable = {context_provider_dbus_manager_dbus_register_object};
static const DBusObjectPathVTable _context_provider_dbus_subscriber_dbus_path_vtable = {_context_provider_dbus_subscriber_dbus_unregister, context_provider_dbus_subscriber_dbus_message};
static const _DBusObjectVTable _context_provider_dbus_subscriber_dbus_vtable = {context_provider_dbus_subscriber_dbus_register_object};

static void g_cclosure_user_marshal_VOID__BOXED_BOXED_INT (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);

void context_provider_dbus_manager_Get (ContextProviderDBusManager* self, char** keys, int keys_length1, GHashTable** values, char*** undeterminable_keys, int* undeterminable_keys_length1) {
	CONTEXT_PROVIDER_DBUS_MANAGER_GET_INTERFACE (self)->Get (self, keys, keys_length1, values, undeterminable_keys, undeterminable_keys_length1);
}


char* context_provider_dbus_manager_GetSubscriber (ContextProviderDBusManager* self, const char* name, GError** error) {
	return CONTEXT_PROVIDER_DBUS_MANAGER_GET_INTERFACE (self)->GetSubscriber (self, name, error);
}


void _context_provider_dbus_manager_dbus_unregister (DBusConnection* connection, void* user_data) {
}


static DBusMessage* _dbus_context_provider_dbus_manager_introspect (ContextProviderDBusManager* self, DBusConnection* connection, DBusMessage* message) {
	DBusMessage* reply;
	DBusMessageIter iter;
	GString* xml_data;
	char** children;
	int i;
	reply = dbus_message_new_method_return (message);
	dbus_message_iter_init_append (reply, &iter);
	xml_data = g_string_new ("<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n");
	g_string_append (xml_data, "<node>\n<interface name=\"org.freedesktop.DBus.Introspectable\">\n  <method name=\"Introspect\">\n    <arg name=\"data\" direction=\"out\" type=\"s\"/>\n  </method>\n</interface>\n<interface name=\"org.freedesktop.DBus.Properties\">\n  <method name=\"Get\">\n    <arg name=\"interface\" direction=\"in\" type=\"s\"/>\n    <arg name=\"propname\" direction=\"in\" type=\"s\"/>\n    <arg name=\"value\" direction=\"out\" type=\"v\"/>\n  </method>\n  <method name=\"Set\">\n    <arg name=\"interface\" direction=\"in\" type=\"s\"/>\n    <arg name=\"propname\" direction=\"in\" type=\"s\"/>\n    <arg name=\"value\" direction=\"in\" type=\"v\"/>\n  </method>\n  <method name=\"GetAll\">\n    <arg name=\"interface\" direction=\"in\" type=\"s\"/>\n    <arg name=\"props\" direction=\"out\" type=\"a{sv}\"/>\n  </method>\n</interface>\n<interface name=\"org.freedesktop.ContextKit.Manager\">\n  <method name=\"Get\">\n    <arg name=\"keys\" type=\"as\" direction=\"in\"/>\n    <arg name=\"values\" type=\"a{sv}\" direction=\"out\"/>\n    <arg name=\"undeterminable_keys\" type=\"as\" direction=\"out\"/>\n  </method>\n  <method name=\"GetSubscriber\">\n    <arg name=\"result\" type=\"o\" direction=\"out\"/>\n  </method>\n</interface>\n");
	dbus_connection_list_registered (connection, g_object_get_data ((GObject *) self, "dbus_object_path"), &children);
	for (i = 0; children[i]; i++) {
		g_string_append_printf (xml_data, "<node name=\"%s\"/>\n", children[i]);
	}
	g_string_append (xml_data, "</node>\n");
	dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &xml_data->str);
	g_string_free (xml_data, TRUE);
	return reply;
}


static DBusMessage* _dbus_context_provider_dbus_manager_property_get_all (ContextProviderDBusManager* self, DBusConnection* connection, DBusMessage* message) {
	DBusMessage* reply;
	DBusMessageIter iter, reply_iter, subiter;
	char* interface_name;
	const char* _tmp0;
	if (strcmp (dbus_message_get_signature (message), "s")) {
		return NULL;
	}
	dbus_message_iter_init (message, &iter);
	reply = dbus_message_new_method_return (message);
	dbus_message_iter_init_append (reply, &reply_iter);
	dbus_message_iter_get_basic (&iter, &_tmp0);
	dbus_message_iter_next (&iter);
	interface_name = g_strdup (_tmp0);
	if (strcmp (interface_name, "org.freedesktop.ContextKit.Manager") == 0) {
		dbus_message_iter_open_container (&reply_iter, DBUS_TYPE_ARRAY, "{sv}", &subiter);
		dbus_message_iter_close_container (&reply_iter, &subiter);
	} else {
		return NULL;
	}
	return reply;
}


static DBusMessage* _dbus_context_provider_dbus_manager_Get (ContextProviderDBusManager* self, DBusConnection* connection, DBusMessage* message) {
	DBusMessageIter iter;
	GError* error;
	char** keys;
	int keys_length1;
	char** _tmp1;
	int _tmp1_length;
	int _tmp1_size;
	int _tmp1_length1;
	DBusMessageIter _tmp2;
	GHashTable* values;
	char** undeterminable_keys;
	int undeterminable_keys_length1;
	DBusMessage* reply;
	DBusMessageIter _tmp4, _tmp5;
	GHashTableIter _tmp6;
	gpointer _tmp7, _tmp8;
	char** _tmp19;
	DBusMessageIter _tmp20;
	int _tmp21;
	error = NULL;
	if (strcmp (dbus_message_get_signature (message), "as")) {
		return NULL;
	}
	dbus_message_iter_init (message, &iter);
	keys = NULL;
	keys_length1 = 0;
	_tmp1 = g_new (char*, 5);
	_tmp1_length = 0;
	_tmp1_size = 4;
	_tmp1_length1 = 0;
	dbus_message_iter_recurse (&iter, &_tmp2);
	for (; dbus_message_iter_get_arg_type (&_tmp2); _tmp1_length1++) {
		const char* _tmp3;
		if (_tmp1_size == _tmp1_length) {
			_tmp1_size = 2 * _tmp1_size;
			_tmp1 = g_renew (char*, _tmp1, _tmp1_size + 1);
		}
		dbus_message_iter_get_basic (&_tmp2, &_tmp3);
		dbus_message_iter_next (&_tmp2);
		_tmp1[_tmp1_length++] = g_strdup (_tmp3);
	}
	keys_length1 = _tmp1_length1;
	_tmp1[_tmp1_length] = NULL;
	dbus_message_iter_next (&iter);
	keys = _tmp1;
	values = NULL;
	undeterminable_keys = NULL;
	undeterminable_keys_length1 = 0;
	context_provider_dbus_manager_Get (self, keys, keys_length1, &values, &undeterminable_keys, &undeterminable_keys_length1);
	reply = dbus_message_new_method_return (message);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "{sv}", &_tmp4);
	g_hash_table_iter_init (&_tmp6, values);
	while (g_hash_table_iter_next (&_tmp6, &_tmp7, &_tmp8)) {
		char* _key;
		GValue* _value;
		const char* _tmp9;
		DBusMessageIter _tmp10;
		dbus_message_iter_open_container (&_tmp4, DBUS_TYPE_DICT_ENTRY, NULL, &_tmp5);
		_key = (char*) _tmp7;
		_value = (GValue*) _tmp8;
		_tmp9 = _key;
		dbus_message_iter_append_basic (&_tmp5, DBUS_TYPE_STRING, &_tmp9);
		if (G_VALUE_TYPE (&(*_value)) == G_TYPE_UCHAR) {
			guint8 _tmp11;
			dbus_message_iter_open_container (&_tmp5, DBUS_TYPE_VARIANT, "y", &_tmp10);
			_tmp11 = g_value_get_uchar (&(*_value));
			dbus_message_iter_append_basic (&_tmp10, DBUS_TYPE_BYTE, &_tmp11);
			dbus_message_iter_close_container (&_tmp5, &_tmp10);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_BOOLEAN) {
			dbus_bool_t _tmp12;
			dbus_message_iter_open_container (&_tmp5, DBUS_TYPE_VARIANT, "b", &_tmp10);
			_tmp12 = g_value_get_boolean (&(*_value));
			dbus_message_iter_append_basic (&_tmp10, DBUS_TYPE_BOOLEAN, &_tmp12);
			dbus_message_iter_close_container (&_tmp5, &_tmp10);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_INT) {
			dbus_int32_t _tmp13;
			dbus_message_iter_open_container (&_tmp5, DBUS_TYPE_VARIANT, "i", &_tmp10);
			_tmp13 = g_value_get_int (&(*_value));
			dbus_message_iter_append_basic (&_tmp10, DBUS_TYPE_INT32, &_tmp13);
			dbus_message_iter_close_container (&_tmp5, &_tmp10);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_UINT) {
			dbus_uint32_t _tmp14;
			dbus_message_iter_open_container (&_tmp5, DBUS_TYPE_VARIANT, "u", &_tmp10);
			_tmp14 = g_value_get_uint (&(*_value));
			dbus_message_iter_append_basic (&_tmp10, DBUS_TYPE_UINT32, &_tmp14);
			dbus_message_iter_close_container (&_tmp5, &_tmp10);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_INT64) {
			dbus_int64_t _tmp15;
			dbus_message_iter_open_container (&_tmp5, DBUS_TYPE_VARIANT, "x", &_tmp10);
			_tmp15 = g_value_get_int64 (&(*_value));
			dbus_message_iter_append_basic (&_tmp10, DBUS_TYPE_INT64, &_tmp15);
			dbus_message_iter_close_container (&_tmp5, &_tmp10);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_UINT64) {
			dbus_uint64_t _tmp16;
			dbus_message_iter_open_container (&_tmp5, DBUS_TYPE_VARIANT, "t", &_tmp10);
			_tmp16 = g_value_get_uint64 (&(*_value));
			dbus_message_iter_append_basic (&_tmp10, DBUS_TYPE_UINT64, &_tmp16);
			dbus_message_iter_close_container (&_tmp5, &_tmp10);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_DOUBLE) {
			double _tmp17;
			dbus_message_iter_open_container (&_tmp5, DBUS_TYPE_VARIANT, "d", &_tmp10);
			_tmp17 = g_value_get_double (&(*_value));
			dbus_message_iter_append_basic (&_tmp10, DBUS_TYPE_DOUBLE, &_tmp17);
			dbus_message_iter_close_container (&_tmp5, &_tmp10);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_STRING) {
			const char* _tmp18;
			dbus_message_iter_open_container (&_tmp5, DBUS_TYPE_VARIANT, "s", &_tmp10);
			_tmp18 = g_value_get_string (&(*_value));
			dbus_message_iter_append_basic (&_tmp10, DBUS_TYPE_STRING, &_tmp18);
			dbus_message_iter_close_container (&_tmp5, &_tmp10);
		}
		dbus_message_iter_close_container (&_tmp4, &_tmp5);
	}
	dbus_message_iter_close_container (&iter, &_tmp4);
	_tmp19 = undeterminable_keys;
	dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "s", &_tmp20);
	for (_tmp21 = 0; _tmp21 < undeterminable_keys_length1; _tmp21++) {
		const char* _tmp22;
		_tmp22 = *(_tmp19++);
		dbus_message_iter_append_basic (&_tmp20, DBUS_TYPE_STRING, &_tmp22);
	}
	dbus_message_iter_close_container (&iter, &_tmp20);
	return reply;
}


static DBusMessage* _dbus_context_provider_dbus_manager_GetSubscriber (ContextProviderDBusManager* self, DBusConnection* connection, DBusMessage* message) {
	DBusMessageIter iter;
	GError* error;
	const char* name;
	char* result;
	DBusMessage* reply;
	const char* _tmp23;
	error = NULL;
	if (strcmp (dbus_message_get_signature (message), "")) {
		return NULL;
	}
	dbus_message_iter_init (message, &iter);
	name = NULL;
	result = context_provider_dbus_manager_GetSubscriber (self, dbus_message_get_sender (message), &error);
	if (error) {
		reply = dbus_message_new_error (message, DBUS_ERROR_FAILED, error->message);
		return reply;
	}
	reply = dbus_message_new_method_return (message);
	dbus_message_iter_init_append (reply, &iter);
	_tmp23 = result;
	dbus_message_iter_append_basic (&iter, DBUS_TYPE_OBJECT_PATH, &_tmp23);
	return reply;
}


DBusHandlerResult context_provider_dbus_manager_dbus_message (DBusConnection* connection, DBusMessage* message, void* object) {
	DBusMessage* reply;
	reply = NULL;
	if (dbus_message_is_method_call (message, "org.freedesktop.DBus.Introspectable", "Introspect")) {
		reply = _dbus_context_provider_dbus_manager_introspect (object, connection, message);
	} else if (dbus_message_is_method_call (message, "org.freedesktop.DBus.Properties", "GetAll")) {
		reply = _dbus_context_provider_dbus_manager_property_get_all (object, connection, message);
	} else if (dbus_message_is_method_call (message, "org.freedesktop.ContextKit.Manager", "Get")) {
		reply = _dbus_context_provider_dbus_manager_Get (object, connection, message);
	} else if (dbus_message_is_method_call (message, "org.freedesktop.ContextKit.Manager", "GetSubscriber")) {
		reply = _dbus_context_provider_dbus_manager_GetSubscriber (object, connection, message);
	}
	if (reply) {
		dbus_connection_send (connection, reply, NULL);
		dbus_message_unref (reply);
		return DBUS_HANDLER_RESULT_HANDLED;
	} else {
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}
}


void context_provider_dbus_manager_dbus_register_object (DBusConnection* connection, const char* path, void* object) {
	if (!g_object_get_data (object, "dbus_object_path")) {
		g_object_set_data (object, "dbus_object_path", g_strdup (path));
		dbus_connection_register_object_path (connection, path, &_context_provider_dbus_manager_dbus_path_vtable, object);
	}
}


static void context_provider_dbus_manager_base_init (ContextProviderDBusManagerIface * iface) {
	static gboolean initialized = FALSE;
	if (!initialized) {
		initialized = TRUE;
		g_type_set_qdata (CONTEXT_PROVIDER_TYPE_DBUS_MANAGER, g_quark_from_static_string ("DBusObjectVTable"), (void*) (&_context_provider_dbus_manager_dbus_vtable));
	}
}


GType context_provider_dbus_manager_get_type (void) {
	static GType context_provider_dbus_manager_type_id = 0;
	if (context_provider_dbus_manager_type_id == 0) {
		static const GTypeInfo g_define_type_info = { sizeof (ContextProviderDBusManagerIface), (GBaseInitFunc) context_provider_dbus_manager_base_init, (GBaseFinalizeFunc) NULL, (GClassInitFunc) NULL, (GClassFinalizeFunc) NULL, NULL, 0, 0, (GInstanceInitFunc) NULL, NULL };
		context_provider_dbus_manager_type_id = g_type_register_static (G_TYPE_INTERFACE, "ContextProviderDBusManager", &g_define_type_info, 0);
		g_type_interface_add_prerequisite (context_provider_dbus_manager_type_id, G_TYPE_OBJECT);
	}
	return context_provider_dbus_manager_type_id;
}


G_DEFINE_TYPE_EXTENDED (ContextProviderDBusManagerDBusProxy, context_provider_dbus_manager_dbus_proxy, DBUS_TYPE_G_PROXY, 0, G_IMPLEMENT_INTERFACE (CONTEXT_PROVIDER_TYPE_DBUS_MANAGER, context_provider_dbus_manager_dbus_proxy_interface_init));
ContextProviderDBusManager* context_provider_dbus_manager_dbus_proxy_new (DBusGConnection* connection, const char* name, const char* path) {
	ContextProviderDBusManager* self;
	char* filter;
	self = g_object_new (context_provider_dbus_manager_dbus_proxy_get_type (), "connection", connection, "name", name, "path", path, "interface", "org.freedesktop.ContextKit.Manager", NULL);
	dbus_connection_add_filter (dbus_g_connection_get_connection (connection), context_provider_dbus_manager_dbus_proxy_filter, self, NULL);
	filter = g_strdup_printf ("type='signal',path='%s'", path);
	dbus_bus_add_match (dbus_g_connection_get_connection (connection), filter, NULL);
	g_free (filter);
	return self;
}


DBusHandlerResult context_provider_dbus_manager_dbus_proxy_filter (DBusConnection* connection, DBusMessage* message, void* user_data) {
	if (dbus_message_has_path (message, dbus_g_proxy_get_path (user_data))) {
	}
	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}


static void context_provider_dbus_manager_dbus_proxy_dispose (GObject* self) {
	DBusGConnection *connection;
	g_object_get (self, "connection", &connection, NULL);
	dbus_connection_remove_filter (dbus_g_connection_get_connection (connection), context_provider_dbus_manager_dbus_proxy_filter, self);
	G_OBJECT_CLASS (context_provider_dbus_manager_dbus_proxy_parent_class)->dispose (self);
}


static void context_provider_dbus_manager_dbus_proxy_class_init (ContextProviderDBusManagerDBusProxyClass* klass) {
	G_OBJECT_CLASS (klass)->dispose = context_provider_dbus_manager_dbus_proxy_dispose;
}


static void context_provider_dbus_manager_dbus_proxy_init (ContextProviderDBusManagerDBusProxy* self) {
}


static void context_provider_dbus_manager_dbus_proxy_Get (ContextProviderDBusManager* self, char** keys, int keys_length1, GHashTable** values, char*** undeterminable_keys, int* undeterminable_keys_length1) {
	DBusGConnection *_connection;
	DBusMessage *_message, *_reply;
	DBusMessageIter _iter;
	char** _tmp24;
	DBusMessageIter _tmp25;
	int _tmp26;
	GHashTable* _values;
	GHashTable* _tmp28;
	DBusMessageIter _tmp29;
	DBusMessageIter _tmp30;
	char** _undeterminable_keys;
	int _undeterminable_keys_length1;
	char** _tmp46;
	int _tmp46_length;
	int _tmp46_size;
	int _tmp46_length1;
	DBusMessageIter _tmp47;
	_message = dbus_message_new_method_call (dbus_g_proxy_get_bus_name ((DBusGProxy*) self), dbus_g_proxy_get_path ((DBusGProxy*) self), "org.freedesktop.ContextKit.Manager", "Get");
	dbus_message_iter_init_append (_message, &_iter);
	_tmp24 = keys;
	dbus_message_iter_open_container (&_iter, DBUS_TYPE_ARRAY, "s", &_tmp25);
	for (_tmp26 = 0; _tmp26 < keys_length1; _tmp26++) {
		const char* _tmp27;
		_tmp27 = *(_tmp24++);
		dbus_message_iter_append_basic (&_tmp25, DBUS_TYPE_STRING, &_tmp27);
	}
	dbus_message_iter_close_container (&_iter, &_tmp25);
	g_object_get (self, "connection", &_connection, NULL);
	_reply = dbus_connection_send_with_reply_and_block (dbus_g_connection_get_connection (_connection), _message, -1, NULL);
	dbus_g_connection_unref (_connection);
	dbus_message_unref (_message);
	dbus_message_iter_init (_reply, &_iter);
	_tmp28 = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
	dbus_message_iter_recurse (&_iter, &_tmp29);
	while (dbus_message_iter_get_arg_type (&_tmp29)) {
		char* _key;
		GValue* _value;
		const char* _tmp31;
		GValue _tmp32 = {0};
		DBusMessageIter _tmp33;
		dbus_message_iter_recurse (&_tmp29, &_tmp30);
		dbus_message_iter_get_basic (&_tmp30, &_tmp31);
		dbus_message_iter_next (&_tmp30);
		_key = g_strdup (_tmp31);
		dbus_message_iter_recurse (&_tmp30, &_tmp33);
		if (dbus_message_iter_get_arg_type (&_tmp33) == DBUS_TYPE_BYTE) {
			guint8 _tmp34;
			dbus_message_iter_get_basic (&_tmp33, &_tmp34);
			g_value_init (&_tmp32, G_TYPE_UCHAR);
			g_value_set_uchar (&_tmp32, _tmp34);
		} else if (dbus_message_iter_get_arg_type (&_tmp33) == DBUS_TYPE_BOOLEAN) {
			dbus_bool_t _tmp35;
			dbus_message_iter_get_basic (&_tmp33, &_tmp35);
			g_value_init (&_tmp32, G_TYPE_BOOLEAN);
			g_value_set_boolean (&_tmp32, _tmp35);
		} else if (dbus_message_iter_get_arg_type (&_tmp33) == DBUS_TYPE_INT16) {
			dbus_int16_t _tmp36;
			dbus_message_iter_get_basic (&_tmp33, &_tmp36);
			g_value_init (&_tmp32, G_TYPE_INT);
			g_value_set_int (&_tmp32, _tmp36);
		} else if (dbus_message_iter_get_arg_type (&_tmp33) == DBUS_TYPE_UINT16) {
			dbus_uint16_t _tmp37;
			dbus_message_iter_get_basic (&_tmp33, &_tmp37);
			g_value_init (&_tmp32, G_TYPE_UINT);
			g_value_set_uint (&_tmp32, _tmp37);
		} else if (dbus_message_iter_get_arg_type (&_tmp33) == DBUS_TYPE_INT32) {
			dbus_int32_t _tmp38;
			dbus_message_iter_get_basic (&_tmp33, &_tmp38);
			g_value_init (&_tmp32, G_TYPE_INT);
			g_value_set_int (&_tmp32, _tmp38);
		} else if (dbus_message_iter_get_arg_type (&_tmp33) == DBUS_TYPE_UINT32) {
			dbus_uint32_t _tmp39;
			dbus_message_iter_get_basic (&_tmp33, &_tmp39);
			g_value_init (&_tmp32, G_TYPE_UINT);
			g_value_set_uint (&_tmp32, _tmp39);
		} else if (dbus_message_iter_get_arg_type (&_tmp33) == DBUS_TYPE_INT64) {
			dbus_int64_t _tmp40;
			dbus_message_iter_get_basic (&_tmp33, &_tmp40);
			g_value_init (&_tmp32, G_TYPE_INT64);
			g_value_set_int64 (&_tmp32, _tmp40);
		} else if (dbus_message_iter_get_arg_type (&_tmp33) == DBUS_TYPE_UINT64) {
			dbus_uint64_t _tmp41;
			dbus_message_iter_get_basic (&_tmp33, &_tmp41);
			g_value_init (&_tmp32, G_TYPE_UINT64);
			g_value_set_uint64 (&_tmp32, _tmp41);
		} else if (dbus_message_iter_get_arg_type (&_tmp33) == DBUS_TYPE_DOUBLE) {
			double _tmp42;
			dbus_message_iter_get_basic (&_tmp33, &_tmp42);
			g_value_init (&_tmp32, G_TYPE_DOUBLE);
			g_value_set_double (&_tmp32, _tmp42);
		} else if (dbus_message_iter_get_arg_type (&_tmp33) == DBUS_TYPE_STRING) {
			const char* _tmp43;
			dbus_message_iter_get_basic (&_tmp33, &_tmp43);
			g_value_init (&_tmp32, G_TYPE_STRING);
			g_value_take_string (&_tmp32, g_strdup (_tmp43));
		} else if (dbus_message_iter_get_arg_type (&_tmp33) == DBUS_TYPE_OBJECT_PATH) {
			const char* _tmp44;
			dbus_message_iter_get_basic (&_tmp33, &_tmp44);
			g_value_init (&_tmp32, G_TYPE_STRING);
			g_value_take_string (&_tmp32, g_strdup (_tmp44));
		} else if (dbus_message_iter_get_arg_type (&_tmp33) == DBUS_TYPE_SIGNATURE) {
			const char* _tmp45;
			dbus_message_iter_get_basic (&_tmp33, &_tmp45);
			g_value_init (&_tmp32, G_TYPE_STRING);
			g_value_take_string (&_tmp32, g_strdup (_tmp45));
		}
		dbus_message_iter_next (&_tmp30);
		_value = g_memdup (&_tmp32, sizeof (GValue));
		g_hash_table_insert (_tmp28, _key, _value);
		dbus_message_iter_next (&_tmp29);
	}
	dbus_message_iter_next (&_iter);
	_values = _tmp28;
	*values = _values;
	_undeterminable_keys_length1 = 0;
	_tmp46 = g_new (char*, 5);
	_tmp46_length = 0;
	_tmp46_size = 4;
	_tmp46_length1 = 0;
	dbus_message_iter_recurse (&_iter, &_tmp47);
	for (; dbus_message_iter_get_arg_type (&_tmp47); _tmp46_length1++) {
		const char* _tmp48;
		if (_tmp46_size == _tmp46_length) {
			_tmp46_size = 2 * _tmp46_size;
			_tmp46 = g_renew (char*, _tmp46, _tmp46_size + 1);
		}
		dbus_message_iter_get_basic (&_tmp47, &_tmp48);
		dbus_message_iter_next (&_tmp47);
		_tmp46[_tmp46_length++] = g_strdup (_tmp48);
	}
	_undeterminable_keys_length1 = _tmp46_length1;
	_tmp46[_tmp46_length] = NULL;
	dbus_message_iter_next (&_iter);
	_undeterminable_keys = _tmp46;
	*undeterminable_keys = _undeterminable_keys;
	*undeterminable_keys_length1 = _undeterminable_keys_length1;
	dbus_message_unref (_reply);
}


static char* context_provider_dbus_manager_dbus_proxy_GetSubscriber (ContextProviderDBusManager* self, const char* name, GError** error) {
	DBusGConnection *_connection;
	DBusMessage *_message, *_reply;
	DBusMessageIter _iter;
	char* _result;
	const char* _tmp49;
	_message = dbus_message_new_method_call (dbus_g_proxy_get_bus_name ((DBusGProxy*) self), dbus_g_proxy_get_path ((DBusGProxy*) self), "org.freedesktop.ContextKit.Manager", "GetSubscriber");
	dbus_message_iter_init_append (_message, &_iter);
	g_object_get (self, "connection", &_connection, NULL);
	_reply = dbus_connection_send_with_reply_and_block (dbus_g_connection_get_connection (_connection), _message, -1, NULL);
	dbus_g_connection_unref (_connection);
	dbus_message_unref (_message);
	dbus_message_iter_init (_reply, &_iter);
	dbus_message_iter_get_basic (&_iter, &_tmp49);
	dbus_message_iter_next (&_iter);
	_result = g_strdup (_tmp49);
	dbus_message_unref (_reply);
	return _result;
}


static void context_provider_dbus_manager_dbus_proxy_interface_init (ContextProviderDBusManagerIface* iface) {
	iface->Get = context_provider_dbus_manager_dbus_proxy_Get;
	iface->GetSubscriber = context_provider_dbus_manager_dbus_proxy_GetSubscriber;
}


void context_provider_dbus_subscriber_Subscribe (ContextProviderDBusSubscriber* self, char** keys, int keys_length1, GHashTable** values, char*** undeterminable_keys, int* undeterminable_keys_length1) {
	CONTEXT_PROVIDER_DBUS_SUBSCRIBER_GET_INTERFACE (self)->Subscribe (self, keys, keys_length1, values, undeterminable_keys, undeterminable_keys_length1);
}


void context_provider_dbus_subscriber_Unsubscribe (ContextProviderDBusSubscriber* self, char** keys, int keys_length1) {
	CONTEXT_PROVIDER_DBUS_SUBSCRIBER_GET_INTERFACE (self)->Unsubscribe (self, keys, keys_length1);
}


void _context_provider_dbus_subscriber_dbus_unregister (DBusConnection* connection, void* user_data) {
}


static DBusMessage* _dbus_context_provider_dbus_subscriber_introspect (ContextProviderDBusSubscriber* self, DBusConnection* connection, DBusMessage* message) {
	DBusMessage* reply;
	DBusMessageIter iter;
	GString* xml_data;
	char** children;
	int i;
	reply = dbus_message_new_method_return (message);
	dbus_message_iter_init_append (reply, &iter);
	xml_data = g_string_new ("<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n");
	g_string_append (xml_data, "<node>\n<interface name=\"org.freedesktop.DBus.Introspectable\">\n  <method name=\"Introspect\">\n    <arg name=\"data\" direction=\"out\" type=\"s\"/>\n  </method>\n</interface>\n<interface name=\"org.freedesktop.DBus.Properties\">\n  <method name=\"Get\">\n    <arg name=\"interface\" direction=\"in\" type=\"s\"/>\n    <arg name=\"propname\" direction=\"in\" type=\"s\"/>\n    <arg name=\"value\" direction=\"out\" type=\"v\"/>\n  </method>\n  <method name=\"Set\">\n    <arg name=\"interface\" direction=\"in\" type=\"s\"/>\n    <arg name=\"propname\" direction=\"in\" type=\"s\"/>\n    <arg name=\"value\" direction=\"in\" type=\"v\"/>\n  </method>\n  <method name=\"GetAll\">\n    <arg name=\"interface\" direction=\"in\" type=\"s\"/>\n    <arg name=\"props\" direction=\"out\" type=\"a{sv}\"/>\n  </method>\n</interface>\n<interface name=\"org.freedesktop.ContextKit.Subscriber\">\n  <method name=\"Subscribe\">\n    <arg name=\"keys\" type=\"as\" direction=\"in\"/>\n    <arg name=\"values\" type=\"a{sv}\" direction=\"out\"/>\n    <arg name=\"undeterminable_keys\" type=\"as\" direction=\"out\"/>\n  </method>\n  <method name=\"Unsubscribe\">\n    <arg name=\"keys\" type=\"as\" direction=\"in\"/>\n  </method>\n  <signal name=\"Changed\">\n    <arg name=\"values\" type=\"a{sv}\"/>\n    <arg name=\"undeterminable_keys\" type=\"as\"/>\n  </signal>\n</interface>\n");
	dbus_connection_list_registered (connection, g_object_get_data ((GObject *) self, "dbus_object_path"), &children);
	for (i = 0; children[i]; i++) {
		g_string_append_printf (xml_data, "<node name=\"%s\"/>\n", children[i]);
	}
	g_string_append (xml_data, "</node>\n");
	dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &xml_data->str);
	g_string_free (xml_data, TRUE);
	return reply;
}


static DBusMessage* _dbus_context_provider_dbus_subscriber_property_get_all (ContextProviderDBusSubscriber* self, DBusConnection* connection, DBusMessage* message) {
	DBusMessage* reply;
	DBusMessageIter iter, reply_iter, subiter;
	char* interface_name;
	const char* _tmp50;
	if (strcmp (dbus_message_get_signature (message), "s")) {
		return NULL;
	}
	dbus_message_iter_init (message, &iter);
	reply = dbus_message_new_method_return (message);
	dbus_message_iter_init_append (reply, &reply_iter);
	dbus_message_iter_get_basic (&iter, &_tmp50);
	dbus_message_iter_next (&iter);
	interface_name = g_strdup (_tmp50);
	if (strcmp (interface_name, "org.freedesktop.ContextKit.Subscriber") == 0) {
		dbus_message_iter_open_container (&reply_iter, DBUS_TYPE_ARRAY, "{sv}", &subiter);
		dbus_message_iter_close_container (&reply_iter, &subiter);
	} else {
		return NULL;
	}
	return reply;
}


static DBusMessage* _dbus_context_provider_dbus_subscriber_Subscribe (ContextProviderDBusSubscriber* self, DBusConnection* connection, DBusMessage* message) {
	DBusMessageIter iter;
	GError* error;
	char** keys;
	int keys_length1;
	char** _tmp51;
	int _tmp51_length;
	int _tmp51_size;
	int _tmp51_length1;
	DBusMessageIter _tmp52;
	GHashTable* values;
	char** undeterminable_keys;
	int undeterminable_keys_length1;
	DBusMessage* reply;
	DBusMessageIter _tmp54, _tmp55;
	GHashTableIter _tmp56;
	gpointer _tmp57, _tmp58;
	char** _tmp69;
	DBusMessageIter _tmp70;
	int _tmp71;
	error = NULL;
	if (strcmp (dbus_message_get_signature (message), "as")) {
		return NULL;
	}
	dbus_message_iter_init (message, &iter);
	keys = NULL;
	keys_length1 = 0;
	_tmp51 = g_new (char*, 5);
	_tmp51_length = 0;
	_tmp51_size = 4;
	_tmp51_length1 = 0;
	dbus_message_iter_recurse (&iter, &_tmp52);
	for (; dbus_message_iter_get_arg_type (&_tmp52); _tmp51_length1++) {
		const char* _tmp53;
		if (_tmp51_size == _tmp51_length) {
			_tmp51_size = 2 * _tmp51_size;
			_tmp51 = g_renew (char*, _tmp51, _tmp51_size + 1);
		}
		dbus_message_iter_get_basic (&_tmp52, &_tmp53);
		dbus_message_iter_next (&_tmp52);
		_tmp51[_tmp51_length++] = g_strdup (_tmp53);
	}
	keys_length1 = _tmp51_length1;
	_tmp51[_tmp51_length] = NULL;
	dbus_message_iter_next (&iter);
	keys = _tmp51;
	values = NULL;
	undeterminable_keys = NULL;
	undeterminable_keys_length1 = 0;
	context_provider_dbus_subscriber_Subscribe (self, keys, keys_length1, &values, &undeterminable_keys, &undeterminable_keys_length1);
	reply = dbus_message_new_method_return (message);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "{sv}", &_tmp54);
	g_hash_table_iter_init (&_tmp56, values);
	while (g_hash_table_iter_next (&_tmp56, &_tmp57, &_tmp58)) {
		char* _key;
		GValue* _value;
		const char* _tmp59;
		DBusMessageIter _tmp60;
		dbus_message_iter_open_container (&_tmp54, DBUS_TYPE_DICT_ENTRY, NULL, &_tmp55);
		_key = (char*) _tmp57;
		_value = (GValue*) _tmp58;
		_tmp59 = _key;
		dbus_message_iter_append_basic (&_tmp55, DBUS_TYPE_STRING, &_tmp59);
		if (G_VALUE_TYPE (&(*_value)) == G_TYPE_UCHAR) {
			guint8 _tmp61;
			dbus_message_iter_open_container (&_tmp55, DBUS_TYPE_VARIANT, "y", &_tmp60);
			_tmp61 = g_value_get_uchar (&(*_value));
			dbus_message_iter_append_basic (&_tmp60, DBUS_TYPE_BYTE, &_tmp61);
			dbus_message_iter_close_container (&_tmp55, &_tmp60);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_BOOLEAN) {
			dbus_bool_t _tmp62;
			dbus_message_iter_open_container (&_tmp55, DBUS_TYPE_VARIANT, "b", &_tmp60);
			_tmp62 = g_value_get_boolean (&(*_value));
			dbus_message_iter_append_basic (&_tmp60, DBUS_TYPE_BOOLEAN, &_tmp62);
			dbus_message_iter_close_container (&_tmp55, &_tmp60);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_INT) {
			dbus_int32_t _tmp63;
			dbus_message_iter_open_container (&_tmp55, DBUS_TYPE_VARIANT, "i", &_tmp60);
			_tmp63 = g_value_get_int (&(*_value));
			dbus_message_iter_append_basic (&_tmp60, DBUS_TYPE_INT32, &_tmp63);
			dbus_message_iter_close_container (&_tmp55, &_tmp60);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_UINT) {
			dbus_uint32_t _tmp64;
			dbus_message_iter_open_container (&_tmp55, DBUS_TYPE_VARIANT, "u", &_tmp60);
			_tmp64 = g_value_get_uint (&(*_value));
			dbus_message_iter_append_basic (&_tmp60, DBUS_TYPE_UINT32, &_tmp64);
			dbus_message_iter_close_container (&_tmp55, &_tmp60);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_INT64) {
			dbus_int64_t _tmp65;
			dbus_message_iter_open_container (&_tmp55, DBUS_TYPE_VARIANT, "x", &_tmp60);
			_tmp65 = g_value_get_int64 (&(*_value));
			dbus_message_iter_append_basic (&_tmp60, DBUS_TYPE_INT64, &_tmp65);
			dbus_message_iter_close_container (&_tmp55, &_tmp60);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_UINT64) {
			dbus_uint64_t _tmp66;
			dbus_message_iter_open_container (&_tmp55, DBUS_TYPE_VARIANT, "t", &_tmp60);
			_tmp66 = g_value_get_uint64 (&(*_value));
			dbus_message_iter_append_basic (&_tmp60, DBUS_TYPE_UINT64, &_tmp66);
			dbus_message_iter_close_container (&_tmp55, &_tmp60);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_DOUBLE) {
			double _tmp67;
			dbus_message_iter_open_container (&_tmp55, DBUS_TYPE_VARIANT, "d", &_tmp60);
			_tmp67 = g_value_get_double (&(*_value));
			dbus_message_iter_append_basic (&_tmp60, DBUS_TYPE_DOUBLE, &_tmp67);
			dbus_message_iter_close_container (&_tmp55, &_tmp60);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_STRING) {
			const char* _tmp68;
			dbus_message_iter_open_container (&_tmp55, DBUS_TYPE_VARIANT, "s", &_tmp60);
			_tmp68 = g_value_get_string (&(*_value));
			dbus_message_iter_append_basic (&_tmp60, DBUS_TYPE_STRING, &_tmp68);
			dbus_message_iter_close_container (&_tmp55, &_tmp60);
		}
		dbus_message_iter_close_container (&_tmp54, &_tmp55);
	}
	dbus_message_iter_close_container (&iter, &_tmp54);
	_tmp69 = undeterminable_keys;
	dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "s", &_tmp70);
	for (_tmp71 = 0; _tmp71 < undeterminable_keys_length1; _tmp71++) {
		const char* _tmp72;
		_tmp72 = *(_tmp69++);
		dbus_message_iter_append_basic (&_tmp70, DBUS_TYPE_STRING, &_tmp72);
	}
	dbus_message_iter_close_container (&iter, &_tmp70);
	return reply;
}


static DBusMessage* _dbus_context_provider_dbus_subscriber_Unsubscribe (ContextProviderDBusSubscriber* self, DBusConnection* connection, DBusMessage* message) {
	DBusMessageIter iter;
	GError* error;
	char** keys;
	int keys_length1;
	char** _tmp73;
	int _tmp73_length;
	int _tmp73_size;
	int _tmp73_length1;
	DBusMessageIter _tmp74;
	DBusMessage* reply;
	error = NULL;
	if (strcmp (dbus_message_get_signature (message), "as")) {
		return NULL;
	}
	dbus_message_iter_init (message, &iter);
	keys = NULL;
	keys_length1 = 0;
	_tmp73 = g_new (char*, 5);
	_tmp73_length = 0;
	_tmp73_size = 4;
	_tmp73_length1 = 0;
	dbus_message_iter_recurse (&iter, &_tmp74);
	for (; dbus_message_iter_get_arg_type (&_tmp74); _tmp73_length1++) {
		const char* _tmp75;
		if (_tmp73_size == _tmp73_length) {
			_tmp73_size = 2 * _tmp73_size;
			_tmp73 = g_renew (char*, _tmp73, _tmp73_size + 1);
		}
		dbus_message_iter_get_basic (&_tmp74, &_tmp75);
		dbus_message_iter_next (&_tmp74);
		_tmp73[_tmp73_length++] = g_strdup (_tmp75);
	}
	keys_length1 = _tmp73_length1;
	_tmp73[_tmp73_length] = NULL;
	dbus_message_iter_next (&iter);
	keys = _tmp73;
	context_provider_dbus_subscriber_Unsubscribe (self, keys, keys_length1);
	reply = dbus_message_new_method_return (message);
	dbus_message_iter_init_append (reply, &iter);
	return reply;
}


DBusHandlerResult context_provider_dbus_subscriber_dbus_message (DBusConnection* connection, DBusMessage* message, void* object) {
	DBusMessage* reply;
	reply = NULL;
	if (dbus_message_is_method_call (message, "org.freedesktop.DBus.Introspectable", "Introspect")) {
		reply = _dbus_context_provider_dbus_subscriber_introspect (object, connection, message);
	} else if (dbus_message_is_method_call (message, "org.freedesktop.DBus.Properties", "GetAll")) {
		reply = _dbus_context_provider_dbus_subscriber_property_get_all (object, connection, message);
	} else if (dbus_message_is_method_call (message, "org.freedesktop.ContextKit.Subscriber", "Subscribe")) {
		reply = _dbus_context_provider_dbus_subscriber_Subscribe (object, connection, message);
	} else if (dbus_message_is_method_call (message, "org.freedesktop.ContextKit.Subscriber", "Unsubscribe")) {
		reply = _dbus_context_provider_dbus_subscriber_Unsubscribe (object, connection, message);
	}
	if (reply) {
		dbus_connection_send (connection, reply, NULL);
		dbus_message_unref (reply);
		return DBUS_HANDLER_RESULT_HANDLED;
	} else {
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}
}


static void _dbus_context_provider_dbus_subscriber_changed (GObject* _sender, GHashTable* values, char** undeterminable_keys, int undeterminable_keys_length1, DBusConnection* _connection) {
	const char * _path;
	DBusMessage *_message;
	DBusMessageIter _iter;
	DBusMessageIter _tmp76, _tmp77;
	GHashTableIter _tmp78;
	gpointer _tmp79, _tmp80;
	char** _tmp91;
	DBusMessageIter _tmp92;
	int _tmp93;
	_path = g_object_get_data (_sender, "dbus_object_path");
	_message = dbus_message_new_signal (_path, "org.freedesktop.ContextKit.Subscriber", "Changed");
	dbus_message_iter_init_append (_message, &_iter);
	dbus_message_iter_open_container (&_iter, DBUS_TYPE_ARRAY, "{sv}", &_tmp76);
	g_hash_table_iter_init (&_tmp78, values);
	while (g_hash_table_iter_next (&_tmp78, &_tmp79, &_tmp80)) {
		char* _key;
		GValue* _value;
		const char* _tmp81;
		DBusMessageIter _tmp82;
		dbus_message_iter_open_container (&_tmp76, DBUS_TYPE_DICT_ENTRY, NULL, &_tmp77);
		_key = (char*) _tmp79;
		_value = (GValue*) _tmp80;
		_tmp81 = _key;
		dbus_message_iter_append_basic (&_tmp77, DBUS_TYPE_STRING, &_tmp81);
		if (G_VALUE_TYPE (&(*_value)) == G_TYPE_UCHAR) {
			guint8 _tmp83;
			dbus_message_iter_open_container (&_tmp77, DBUS_TYPE_VARIANT, "y", &_tmp82);
			_tmp83 = g_value_get_uchar (&(*_value));
			dbus_message_iter_append_basic (&_tmp82, DBUS_TYPE_BYTE, &_tmp83);
			dbus_message_iter_close_container (&_tmp77, &_tmp82);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_BOOLEAN) {
			dbus_bool_t _tmp84;
			dbus_message_iter_open_container (&_tmp77, DBUS_TYPE_VARIANT, "b", &_tmp82);
			_tmp84 = g_value_get_boolean (&(*_value));
			dbus_message_iter_append_basic (&_tmp82, DBUS_TYPE_BOOLEAN, &_tmp84);
			dbus_message_iter_close_container (&_tmp77, &_tmp82);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_INT) {
			dbus_int32_t _tmp85;
			dbus_message_iter_open_container (&_tmp77, DBUS_TYPE_VARIANT, "i", &_tmp82);
			_tmp85 = g_value_get_int (&(*_value));
			dbus_message_iter_append_basic (&_tmp82, DBUS_TYPE_INT32, &_tmp85);
			dbus_message_iter_close_container (&_tmp77, &_tmp82);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_UINT) {
			dbus_uint32_t _tmp86;
			dbus_message_iter_open_container (&_tmp77, DBUS_TYPE_VARIANT, "u", &_tmp82);
			_tmp86 = g_value_get_uint (&(*_value));
			dbus_message_iter_append_basic (&_tmp82, DBUS_TYPE_UINT32, &_tmp86);
			dbus_message_iter_close_container (&_tmp77, &_tmp82);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_INT64) {
			dbus_int64_t _tmp87;
			dbus_message_iter_open_container (&_tmp77, DBUS_TYPE_VARIANT, "x", &_tmp82);
			_tmp87 = g_value_get_int64 (&(*_value));
			dbus_message_iter_append_basic (&_tmp82, DBUS_TYPE_INT64, &_tmp87);
			dbus_message_iter_close_container (&_tmp77, &_tmp82);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_UINT64) {
			dbus_uint64_t _tmp88;
			dbus_message_iter_open_container (&_tmp77, DBUS_TYPE_VARIANT, "t", &_tmp82);
			_tmp88 = g_value_get_uint64 (&(*_value));
			dbus_message_iter_append_basic (&_tmp82, DBUS_TYPE_UINT64, &_tmp88);
			dbus_message_iter_close_container (&_tmp77, &_tmp82);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_DOUBLE) {
			double _tmp89;
			dbus_message_iter_open_container (&_tmp77, DBUS_TYPE_VARIANT, "d", &_tmp82);
			_tmp89 = g_value_get_double (&(*_value));
			dbus_message_iter_append_basic (&_tmp82, DBUS_TYPE_DOUBLE, &_tmp89);
			dbus_message_iter_close_container (&_tmp77, &_tmp82);
		} else if (G_VALUE_TYPE (&(*_value)) == G_TYPE_STRING) {
			const char* _tmp90;
			dbus_message_iter_open_container (&_tmp77, DBUS_TYPE_VARIANT, "s", &_tmp82);
			_tmp90 = g_value_get_string (&(*_value));
			dbus_message_iter_append_basic (&_tmp82, DBUS_TYPE_STRING, &_tmp90);
			dbus_message_iter_close_container (&_tmp77, &_tmp82);
		}
		dbus_message_iter_close_container (&_tmp76, &_tmp77);
	}
	dbus_message_iter_close_container (&_iter, &_tmp76);
	_tmp91 = undeterminable_keys;
	dbus_message_iter_open_container (&_iter, DBUS_TYPE_ARRAY, "s", &_tmp92);
	for (_tmp93 = 0; _tmp93 < undeterminable_keys_length1; _tmp93++) {
		const char* _tmp94;
		_tmp94 = *(_tmp91++);
		dbus_message_iter_append_basic (&_tmp92, DBUS_TYPE_STRING, &_tmp94);
	}
	dbus_message_iter_close_container (&_iter, &_tmp92);
	dbus_connection_send (_connection, _message, NULL);
	dbus_message_unref (_message);
}


void context_provider_dbus_subscriber_dbus_register_object (DBusConnection* connection, const char* path, void* object) {
	if (!g_object_get_data (object, "dbus_object_path")) {
		g_object_set_data (object, "dbus_object_path", g_strdup (path));
		dbus_connection_register_object_path (connection, path, &_context_provider_dbus_subscriber_dbus_path_vtable, object);
	}
	g_signal_connect (object, "changed", (GCallback) _dbus_context_provider_dbus_subscriber_changed, connection);
}


static void context_provider_dbus_subscriber_base_init (ContextProviderDBusSubscriberIface * iface) {
	static gboolean initialized = FALSE;
	if (!initialized) {
		initialized = TRUE;
		g_signal_new ("changed", CONTEXT_PROVIDER_TYPE_DBUS_SUBSCRIBER, G_SIGNAL_RUN_LAST, 0, NULL, NULL, g_cclosure_user_marshal_VOID__BOXED_BOXED_INT, G_TYPE_NONE, 3, G_TYPE_HASH_TABLE, G_TYPE_STRV, G_TYPE_INT);
		g_type_set_qdata (CONTEXT_PROVIDER_TYPE_DBUS_SUBSCRIBER, g_quark_from_static_string ("DBusObjectVTable"), (void*) (&_context_provider_dbus_subscriber_dbus_vtable));
	}
}


GType context_provider_dbus_subscriber_get_type (void) {
	static GType context_provider_dbus_subscriber_type_id = 0;
	if (context_provider_dbus_subscriber_type_id == 0) {
		static const GTypeInfo g_define_type_info = { sizeof (ContextProviderDBusSubscriberIface), (GBaseInitFunc) context_provider_dbus_subscriber_base_init, (GBaseFinalizeFunc) NULL, (GClassInitFunc) NULL, (GClassFinalizeFunc) NULL, NULL, 0, 0, (GInstanceInitFunc) NULL, NULL };
		context_provider_dbus_subscriber_type_id = g_type_register_static (G_TYPE_INTERFACE, "ContextProviderDBusSubscriber", &g_define_type_info, 0);
		g_type_interface_add_prerequisite (context_provider_dbus_subscriber_type_id, G_TYPE_OBJECT);
	}
	return context_provider_dbus_subscriber_type_id;
}


G_DEFINE_TYPE_EXTENDED (ContextProviderDBusSubscriberDBusProxy, context_provider_dbus_subscriber_dbus_proxy, DBUS_TYPE_G_PROXY, 0, G_IMPLEMENT_INTERFACE (CONTEXT_PROVIDER_TYPE_DBUS_SUBSCRIBER, context_provider_dbus_subscriber_dbus_proxy_interface_init));
ContextProviderDBusSubscriber* context_provider_dbus_subscriber_dbus_proxy_new (DBusGConnection* connection, const char* name, const char* path) {
	ContextProviderDBusSubscriber* self;
	char* filter;
	self = g_object_new (context_provider_dbus_subscriber_dbus_proxy_get_type (), "connection", connection, "name", name, "path", path, "interface", "org.freedesktop.ContextKit.Subscriber", NULL);
	dbus_connection_add_filter (dbus_g_connection_get_connection (connection), context_provider_dbus_subscriber_dbus_proxy_filter, self, NULL);
	filter = g_strdup_printf ("type='signal',path='%s'", path);
	dbus_bus_add_match (dbus_g_connection_get_connection (connection), filter, NULL);
	g_free (filter);
	return self;
}


static void _dbus_handle_context_provider_dbus_subscriber_changed (ContextProviderDBusSubscriber* self, DBusConnection* connection, DBusMessage* message) {
	DBusMessageIter iter;
	GHashTable* values;
	GHashTable* _tmp95;
	DBusMessageIter _tmp96;
	DBusMessageIter _tmp97;
	char** undeterminable_keys;
	int undeterminable_keys_length1;
	char** _tmp113;
	int _tmp113_length;
	int _tmp113_size;
	int _tmp113_length1;
	DBusMessageIter _tmp114;
	DBusMessage* reply;
	if (strcmp (dbus_message_get_signature (message), "a{sv}as")) {
		return;
	}
	dbus_message_iter_init (message, &iter);
	values = NULL;
	_tmp95 = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
	dbus_message_iter_recurse (&iter, &_tmp96);
	while (dbus_message_iter_get_arg_type (&_tmp96)) {
		char* _key;
		GValue* _value;
		const char* _tmp98;
		GValue _tmp99 = {0};
		DBusMessageIter _tmp100;
		dbus_message_iter_recurse (&_tmp96, &_tmp97);
		dbus_message_iter_get_basic (&_tmp97, &_tmp98);
		dbus_message_iter_next (&_tmp97);
		_key = g_strdup (_tmp98);
		dbus_message_iter_recurse (&_tmp97, &_tmp100);
		if (dbus_message_iter_get_arg_type (&_tmp100) == DBUS_TYPE_BYTE) {
			guint8 _tmp101;
			dbus_message_iter_get_basic (&_tmp100, &_tmp101);
			g_value_init (&_tmp99, G_TYPE_UCHAR);
			g_value_set_uchar (&_tmp99, _tmp101);
		} else if (dbus_message_iter_get_arg_type (&_tmp100) == DBUS_TYPE_BOOLEAN) {
			dbus_bool_t _tmp102;
			dbus_message_iter_get_basic (&_tmp100, &_tmp102);
			g_value_init (&_tmp99, G_TYPE_BOOLEAN);
			g_value_set_boolean (&_tmp99, _tmp102);
		} else if (dbus_message_iter_get_arg_type (&_tmp100) == DBUS_TYPE_INT16) {
			dbus_int16_t _tmp103;
			dbus_message_iter_get_basic (&_tmp100, &_tmp103);
			g_value_init (&_tmp99, G_TYPE_INT);
			g_value_set_int (&_tmp99, _tmp103);
		} else if (dbus_message_iter_get_arg_type (&_tmp100) == DBUS_TYPE_UINT16) {
			dbus_uint16_t _tmp104;
			dbus_message_iter_get_basic (&_tmp100, &_tmp104);
			g_value_init (&_tmp99, G_TYPE_UINT);
			g_value_set_uint (&_tmp99, _tmp104);
		} else if (dbus_message_iter_get_arg_type (&_tmp100) == DBUS_TYPE_INT32) {
			dbus_int32_t _tmp105;
			dbus_message_iter_get_basic (&_tmp100, &_tmp105);
			g_value_init (&_tmp99, G_TYPE_INT);
			g_value_set_int (&_tmp99, _tmp105);
		} else if (dbus_message_iter_get_arg_type (&_tmp100) == DBUS_TYPE_UINT32) {
			dbus_uint32_t _tmp106;
			dbus_message_iter_get_basic (&_tmp100, &_tmp106);
			g_value_init (&_tmp99, G_TYPE_UINT);
			g_value_set_uint (&_tmp99, _tmp106);
		} else if (dbus_message_iter_get_arg_type (&_tmp100) == DBUS_TYPE_INT64) {
			dbus_int64_t _tmp107;
			dbus_message_iter_get_basic (&_tmp100, &_tmp107);
			g_value_init (&_tmp99, G_TYPE_INT64);
			g_value_set_int64 (&_tmp99, _tmp107);
		} else if (dbus_message_iter_get_arg_type (&_tmp100) == DBUS_TYPE_UINT64) {
			dbus_uint64_t _tmp108;
			dbus_message_iter_get_basic (&_tmp100, &_tmp108);
			g_value_init (&_tmp99, G_TYPE_UINT64);
			g_value_set_uint64 (&_tmp99, _tmp108);
		} else if (dbus_message_iter_get_arg_type (&_tmp100) == DBUS_TYPE_DOUBLE) {
			double _tmp109;
			dbus_message_iter_get_basic (&_tmp100, &_tmp109);
			g_value_init (&_tmp99, G_TYPE_DOUBLE);
			g_value_set_double (&_tmp99, _tmp109);
		} else if (dbus_message_iter_get_arg_type (&_tmp100) == DBUS_TYPE_STRING) {
			const char* _tmp110;
			dbus_message_iter_get_basic (&_tmp100, &_tmp110);
			g_value_init (&_tmp99, G_TYPE_STRING);
			g_value_take_string (&_tmp99, g_strdup (_tmp110));
		} else if (dbus_message_iter_get_arg_type (&_tmp100) == DBUS_TYPE_OBJECT_PATH) {
			const char* _tmp111;
			dbus_message_iter_get_basic (&_tmp100, &_tmp111);
			g_value_init (&_tmp99, G_TYPE_STRING);
			g_value_take_string (&_tmp99, g_strdup (_tmp111));
		} else if (dbus_message_iter_get_arg_type (&_tmp100) == DBUS_TYPE_SIGNATURE) {
			const char* _tmp112;
			dbus_message_iter_get_basic (&_tmp100, &_tmp112);
			g_value_init (&_tmp99, G_TYPE_STRING);
			g_value_take_string (&_tmp99, g_strdup (_tmp112));
		}
		dbus_message_iter_next (&_tmp97);
		_value = g_memdup (&_tmp99, sizeof (GValue));
		g_hash_table_insert (_tmp95, _key, _value);
		dbus_message_iter_next (&_tmp96);
	}
	dbus_message_iter_next (&iter);
	values = _tmp95;
	undeterminable_keys = NULL;
	undeterminable_keys_length1 = 0;
	_tmp113 = g_new (char*, 5);
	_tmp113_length = 0;
	_tmp113_size = 4;
	_tmp113_length1 = 0;
	dbus_message_iter_recurse (&iter, &_tmp114);
	for (; dbus_message_iter_get_arg_type (&_tmp114); _tmp113_length1++) {
		const char* _tmp115;
		if (_tmp113_size == _tmp113_length) {
			_tmp113_size = 2 * _tmp113_size;
			_tmp113 = g_renew (char*, _tmp113, _tmp113_size + 1);
		}
		dbus_message_iter_get_basic (&_tmp114, &_tmp115);
		dbus_message_iter_next (&_tmp114);
		_tmp113[_tmp113_length++] = g_strdup (_tmp115);
	}
	undeterminable_keys_length1 = _tmp113_length1;
	_tmp113[_tmp113_length] = NULL;
	dbus_message_iter_next (&iter);
	undeterminable_keys = _tmp113;
	g_signal_emit_by_name (self, "changed", values, undeterminable_keys, undeterminable_keys_length1);
}


DBusHandlerResult context_provider_dbus_subscriber_dbus_proxy_filter (DBusConnection* connection, DBusMessage* message, void* user_data) {
	if (dbus_message_has_path (message, dbus_g_proxy_get_path (user_data))) {
		if (dbus_message_is_signal (message, "org.freedesktop.ContextKit.Subscriber", "Changed")) {
			_dbus_handle_context_provider_dbus_subscriber_changed (user_data, connection, message);
		}
	}
	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}


static void context_provider_dbus_subscriber_dbus_proxy_dispose (GObject* self) {
	DBusGConnection *connection;
	g_object_get (self, "connection", &connection, NULL);
	dbus_connection_remove_filter (dbus_g_connection_get_connection (connection), context_provider_dbus_subscriber_dbus_proxy_filter, self);
	G_OBJECT_CLASS (context_provider_dbus_subscriber_dbus_proxy_parent_class)->dispose (self);
}


static void context_provider_dbus_subscriber_dbus_proxy_class_init (ContextProviderDBusSubscriberDBusProxyClass* klass) {
	G_OBJECT_CLASS (klass)->dispose = context_provider_dbus_subscriber_dbus_proxy_dispose;
}


static void context_provider_dbus_subscriber_dbus_proxy_init (ContextProviderDBusSubscriberDBusProxy* self) {
}


static void context_provider_dbus_subscriber_dbus_proxy_Subscribe (ContextProviderDBusSubscriber* self, char** keys, int keys_length1, GHashTable** values, char*** undeterminable_keys, int* undeterminable_keys_length1) {
	DBusGConnection *_connection;
	DBusMessage *_message, *_reply;
	DBusMessageIter _iter;
	char** _tmp116;
	DBusMessageIter _tmp117;
	int _tmp118;
	GHashTable* _values;
	GHashTable* _tmp120;
	DBusMessageIter _tmp121;
	DBusMessageIter _tmp122;
	char** _undeterminable_keys;
	int _undeterminable_keys_length1;
	char** _tmp138;
	int _tmp138_length;
	int _tmp138_size;
	int _tmp138_length1;
	DBusMessageIter _tmp139;
	_message = dbus_message_new_method_call (dbus_g_proxy_get_bus_name ((DBusGProxy*) self), dbus_g_proxy_get_path ((DBusGProxy*) self), "org.freedesktop.ContextKit.Subscriber", "Subscribe");
	dbus_message_iter_init_append (_message, &_iter);
	_tmp116 = keys;
	dbus_message_iter_open_container (&_iter, DBUS_TYPE_ARRAY, "s", &_tmp117);
	for (_tmp118 = 0; _tmp118 < keys_length1; _tmp118++) {
		const char* _tmp119;
		_tmp119 = *(_tmp116++);
		dbus_message_iter_append_basic (&_tmp117, DBUS_TYPE_STRING, &_tmp119);
	}
	dbus_message_iter_close_container (&_iter, &_tmp117);
	g_object_get (self, "connection", &_connection, NULL);
	_reply = dbus_connection_send_with_reply_and_block (dbus_g_connection_get_connection (_connection), _message, -1, NULL);
	dbus_g_connection_unref (_connection);
	dbus_message_unref (_message);
	dbus_message_iter_init (_reply, &_iter);
	_tmp120 = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
	dbus_message_iter_recurse (&_iter, &_tmp121);
	while (dbus_message_iter_get_arg_type (&_tmp121)) {
		char* _key;
		GValue* _value;
		const char* _tmp123;
		GValue _tmp124 = {0};
		DBusMessageIter _tmp125;
		dbus_message_iter_recurse (&_tmp121, &_tmp122);
		dbus_message_iter_get_basic (&_tmp122, &_tmp123);
		dbus_message_iter_next (&_tmp122);
		_key = g_strdup (_tmp123);
		dbus_message_iter_recurse (&_tmp122, &_tmp125);
		if (dbus_message_iter_get_arg_type (&_tmp125) == DBUS_TYPE_BYTE) {
			guint8 _tmp126;
			dbus_message_iter_get_basic (&_tmp125, &_tmp126);
			g_value_init (&_tmp124, G_TYPE_UCHAR);
			g_value_set_uchar (&_tmp124, _tmp126);
		} else if (dbus_message_iter_get_arg_type (&_tmp125) == DBUS_TYPE_BOOLEAN) {
			dbus_bool_t _tmp127;
			dbus_message_iter_get_basic (&_tmp125, &_tmp127);
			g_value_init (&_tmp124, G_TYPE_BOOLEAN);
			g_value_set_boolean (&_tmp124, _tmp127);
		} else if (dbus_message_iter_get_arg_type (&_tmp125) == DBUS_TYPE_INT16) {
			dbus_int16_t _tmp128;
			dbus_message_iter_get_basic (&_tmp125, &_tmp128);
			g_value_init (&_tmp124, G_TYPE_INT);
			g_value_set_int (&_tmp124, _tmp128);
		} else if (dbus_message_iter_get_arg_type (&_tmp125) == DBUS_TYPE_UINT16) {
			dbus_uint16_t _tmp129;
			dbus_message_iter_get_basic (&_tmp125, &_tmp129);
			g_value_init (&_tmp124, G_TYPE_UINT);
			g_value_set_uint (&_tmp124, _tmp129);
		} else if (dbus_message_iter_get_arg_type (&_tmp125) == DBUS_TYPE_INT32) {
			dbus_int32_t _tmp130;
			dbus_message_iter_get_basic (&_tmp125, &_tmp130);
			g_value_init (&_tmp124, G_TYPE_INT);
			g_value_set_int (&_tmp124, _tmp130);
		} else if (dbus_message_iter_get_arg_type (&_tmp125) == DBUS_TYPE_UINT32) {
			dbus_uint32_t _tmp131;
			dbus_message_iter_get_basic (&_tmp125, &_tmp131);
			g_value_init (&_tmp124, G_TYPE_UINT);
			g_value_set_uint (&_tmp124, _tmp131);
		} else if (dbus_message_iter_get_arg_type (&_tmp125) == DBUS_TYPE_INT64) {
			dbus_int64_t _tmp132;
			dbus_message_iter_get_basic (&_tmp125, &_tmp132);
			g_value_init (&_tmp124, G_TYPE_INT64);
			g_value_set_int64 (&_tmp124, _tmp132);
		} else if (dbus_message_iter_get_arg_type (&_tmp125) == DBUS_TYPE_UINT64) {
			dbus_uint64_t _tmp133;
			dbus_message_iter_get_basic (&_tmp125, &_tmp133);
			g_value_init (&_tmp124, G_TYPE_UINT64);
			g_value_set_uint64 (&_tmp124, _tmp133);
		} else if (dbus_message_iter_get_arg_type (&_tmp125) == DBUS_TYPE_DOUBLE) {
			double _tmp134;
			dbus_message_iter_get_basic (&_tmp125, &_tmp134);
			g_value_init (&_tmp124, G_TYPE_DOUBLE);
			g_value_set_double (&_tmp124, _tmp134);
		} else if (dbus_message_iter_get_arg_type (&_tmp125) == DBUS_TYPE_STRING) {
			const char* _tmp135;
			dbus_message_iter_get_basic (&_tmp125, &_tmp135);
			g_value_init (&_tmp124, G_TYPE_STRING);
			g_value_take_string (&_tmp124, g_strdup (_tmp135));
		} else if (dbus_message_iter_get_arg_type (&_tmp125) == DBUS_TYPE_OBJECT_PATH) {
			const char* _tmp136;
			dbus_message_iter_get_basic (&_tmp125, &_tmp136);
			g_value_init (&_tmp124, G_TYPE_STRING);
			g_value_take_string (&_tmp124, g_strdup (_tmp136));
		} else if (dbus_message_iter_get_arg_type (&_tmp125) == DBUS_TYPE_SIGNATURE) {
			const char* _tmp137;
			dbus_message_iter_get_basic (&_tmp125, &_tmp137);
			g_value_init (&_tmp124, G_TYPE_STRING);
			g_value_take_string (&_tmp124, g_strdup (_tmp137));
		}
		dbus_message_iter_next (&_tmp122);
		_value = g_memdup (&_tmp124, sizeof (GValue));
		g_hash_table_insert (_tmp120, _key, _value);
		dbus_message_iter_next (&_tmp121);
	}
	dbus_message_iter_next (&_iter);
	_values = _tmp120;
	*values = _values;
	_undeterminable_keys_length1 = 0;
	_tmp138 = g_new (char*, 5);
	_tmp138_length = 0;
	_tmp138_size = 4;
	_tmp138_length1 = 0;
	dbus_message_iter_recurse (&_iter, &_tmp139);
	for (; dbus_message_iter_get_arg_type (&_tmp139); _tmp138_length1++) {
		const char* _tmp140;
		if (_tmp138_size == _tmp138_length) {
			_tmp138_size = 2 * _tmp138_size;
			_tmp138 = g_renew (char*, _tmp138, _tmp138_size + 1);
		}
		dbus_message_iter_get_basic (&_tmp139, &_tmp140);
		dbus_message_iter_next (&_tmp139);
		_tmp138[_tmp138_length++] = g_strdup (_tmp140);
	}
	_undeterminable_keys_length1 = _tmp138_length1;
	_tmp138[_tmp138_length] = NULL;
	dbus_message_iter_next (&_iter);
	_undeterminable_keys = _tmp138;
	*undeterminable_keys = _undeterminable_keys;
	*undeterminable_keys_length1 = _undeterminable_keys_length1;
	dbus_message_unref (_reply);
}


static void context_provider_dbus_subscriber_dbus_proxy_Unsubscribe (ContextProviderDBusSubscriber* self, char** keys, int keys_length1) {
	DBusGConnection *_connection;
	DBusMessage *_message, *_reply;
	DBusMessageIter _iter;
	char** _tmp141;
	DBusMessageIter _tmp142;
	int _tmp143;
	_message = dbus_message_new_method_call (dbus_g_proxy_get_bus_name ((DBusGProxy*) self), dbus_g_proxy_get_path ((DBusGProxy*) self), "org.freedesktop.ContextKit.Subscriber", "Unsubscribe");
	dbus_message_iter_init_append (_message, &_iter);
	_tmp141 = keys;
	dbus_message_iter_open_container (&_iter, DBUS_TYPE_ARRAY, "s", &_tmp142);
	for (_tmp143 = 0; _tmp143 < keys_length1; _tmp143++) {
		const char* _tmp144;
		_tmp144 = *(_tmp141++);
		dbus_message_iter_append_basic (&_tmp142, DBUS_TYPE_STRING, &_tmp144);
	}
	dbus_message_iter_close_container (&_iter, &_tmp142);
	g_object_get (self, "connection", &_connection, NULL);
	_reply = dbus_connection_send_with_reply_and_block (dbus_g_connection_get_connection (_connection), _message, -1, NULL);
	dbus_g_connection_unref (_connection);
	dbus_message_unref (_message);
	dbus_message_iter_init (_reply, &_iter);
	dbus_message_unref (_reply);
}


static void context_provider_dbus_subscriber_dbus_proxy_interface_init (ContextProviderDBusSubscriberIface* iface) {
	iface->Subscribe = context_provider_dbus_subscriber_dbus_proxy_Subscribe;
	iface->Unsubscribe = context_provider_dbus_subscriber_dbus_proxy_Unsubscribe;
}


static void _vala_dbus_register_object (DBusConnection* connection, const char* path, void* object) {
	const _DBusObjectVTable * vtable;
	vtable = g_type_get_qdata (G_TYPE_FROM_INSTANCE (object), g_quark_from_static_string ("DBusObjectVTable"));
	if (vtable) {
		vtable->register_object (connection, path, object);
	} else {
		g_warning ("Object does not implement any D-Bus interface");
	}
}



static void g_cclosure_user_marshal_VOID__BOXED_BOXED_INT (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data) {
	typedef void (*GMarshalFunc_VOID__BOXED_BOXED_INT) (gpointer data1, gpointer arg_1, gpointer arg_2, gint arg_3, gpointer data2);
	register GMarshalFunc_VOID__BOXED_BOXED_INT callback;
	register GCClosure * cc;
	register gpointer data1, data2;
	cc = (GCClosure *) closure;
	g_return_if_fail (n_param_values == 4);
	if (G_CCLOSURE_SWAP_DATA (closure)) {
		data1 = closure->data;
		data2 = param_values->data[0].v_pointer;
	} else {
		data1 = param_values->data[0].v_pointer;
		data2 = closure->data;
	}
	callback = (GMarshalFunc_VOID__BOXED_BOXED_INT) (marshal_data ? marshal_data : cc->callback);
	callback (data1, g_value_get_boxed (param_values + 1), g_value_get_boxed (param_values + 2), g_value_get_int (param_values + 3), data2);
}



