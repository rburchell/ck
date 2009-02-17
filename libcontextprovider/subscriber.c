
#include <subscriber.h>
#include <gee/iterable.h>
#include <gee/iterator.h>
#include <gee/collection.h>
#include <string_set.h>

typedef struct _DBusObjectVTable _DBusObjectVTable;

struct _DBusObjectVTable {
	void (*register_object) (DBusConnection*, const char*, void*);
};



struct _ContextProviderSubscriberPrivate {
	char* _object_path;
	ContextProviderStringSet* subscribed_keys;
	ContextProviderKeyUsageCounter* key_counter;
};

#define CONTEXT_PROVIDER_SUBSCRIBER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), CONTEXT_PROVIDER_TYPE_SUBSCRIBER, ContextProviderSubscriberPrivate))
enum  {
	CONTEXT_PROVIDER_SUBSCRIBER_DUMMY_PROPERTY,
	CONTEXT_PROVIDER_SUBSCRIBER_OBJECT_PATH
};
static ContextProviderManager* context_provider_subscriber_manager = NULL;
static void _vala_array_add2 (char*** array, int* length, int* size, char* value);
static void context_provider_subscriber_real_Subscribe (ContextProviderDBusSubscriber* base, char** keys, int keys_length1, GHashTable** values, char*** undeterminable_keys, int* undeterminable_keys_length1);
static void context_provider_subscriber_real_Unsubscribe (ContextProviderDBusSubscriber* base, char** keys, int keys_length1);
static GValue* _g_value_dup (GValue* self);
static gpointer context_provider_subscriber_parent_class = NULL;
void _context_provider_subscriber_dbus_unregister (DBusConnection* connection, void* user_data);
static DBusMessage* _dbus_context_provider_subscriber_introspect (ContextProviderSubscriber* self, DBusConnection* connection, DBusMessage* message);
static ContextProviderDBusSubscriberIface* context_provider_subscriber_context_provider_dbus_subscriber_parent_iface = NULL;
static void context_provider_subscriber_finalize (GObject* obj);
static void _vala_array_free (gpointer array, gint array_length, GDestroyNotify destroy_func);
static void _vala_dbus_register_object (DBusConnection* connection, const char* path, void* object);

static const DBusObjectPathVTable _context_provider_subscriber_dbus_path_vtable = {_context_provider_subscriber_dbus_unregister, context_provider_subscriber_dbus_message};
static const _DBusObjectVTable _context_provider_subscriber_dbus_vtable = {context_provider_subscriber_dbus_register_object};


ContextProviderSubscriber* context_provider_subscriber_construct (GType object_type, ContextProviderManager* manager, ContextProviderKeyUsageCounter* key_counter, gint id) {
	ContextProviderSubscriber * self;
	char* path;
	char* _tmp0;
	ContextProviderManager* _tmp2;
	ContextProviderManager* _tmp1;
	ContextProviderKeyUsageCounter* _tmp4;
	ContextProviderKeyUsageCounter* _tmp3;
	ContextProviderStringSet* _tmp5;
	g_return_val_if_fail (manager != NULL, NULL);
	g_return_val_if_fail (key_counter != NULL, NULL);
	self = g_object_newv (object_type, 0, NULL);
	path = g_strdup_printf ("/org/freedesktop/ContextKit/Subscribers/%d", id);
	_tmp0 = NULL;
	context_provider_subscriber_set_object_path (self, _tmp0 = g_strdup (path));
	_tmp0 = (g_free (_tmp0), NULL);
	_tmp2 = NULL;
	_tmp1 = NULL;
	context_provider_subscriber_manager = (_tmp2 = (_tmp1 = manager, (_tmp1 == NULL) ? NULL : g_object_ref (_tmp1)), (context_provider_subscriber_manager == NULL) ? NULL : (context_provider_subscriber_manager = (g_object_unref (context_provider_subscriber_manager), NULL)), _tmp2);
	_tmp4 = NULL;
	_tmp3 = NULL;
	self->priv->key_counter = (_tmp4 = (_tmp3 = key_counter, (_tmp3 == NULL) ? NULL : context_provider_key_usage_counter_ref (_tmp3)), (self->priv->key_counter == NULL) ? NULL : (self->priv->key_counter = (context_provider_key_usage_counter_unref (self->priv->key_counter), NULL)), _tmp4);
	_tmp5 = NULL;
	self->priv->subscribed_keys = (_tmp5 = context_provider_string_set_new (), (self->priv->subscribed_keys == NULL) ? NULL : (self->priv->subscribed_keys = (context_provider_string_set_unref (self->priv->subscribed_keys), NULL)), _tmp5);
	return self;
}


ContextProviderSubscriber* context_provider_subscriber_new (ContextProviderManager* manager, ContextProviderKeyUsageCounter* key_counter, gint id) {
	return context_provider_subscriber_construct (CONTEXT_PROVIDER_TYPE_SUBSCRIBER, manager, key_counter, id);
}


static void _vala_array_add2 (char*** array, int* length, int* size, char* value) {
	if ((*length) == (*size)) {
		*size = (*size) ? (2 * (*size)) : 4;
		*array = g_renew (char*, *array, (*size) + 1);
	}
	(*array)[(*length)++] = value;
	(*array)[*length] = NULL;
}


/* Emit the Changed signal over DBus*/
void context_provider_subscriber_emit_changed (ContextProviderSubscriber* self, GHashTable* values, GeeArrayList* unavail_l) {
	char** _tmp1;
	gint unavail_size;
	gint unavail_length1;
	char** _tmp0;
	char** unavail;
	g_return_if_fail (self != NULL);
	g_return_if_fail (values != NULL);
	_tmp1 = NULL;
	_tmp0 = NULL;
	unavail = (_tmp1 = (_tmp0 = g_new0 (char*, 0 + 1), _tmp0), unavail_length1 = 0, unavail_size = unavail_length1, _tmp1);
	{
		GeeIterator* _str_it;
		_str_it = gee_iterable_iterator ((GeeIterable*) unavail_l);
		while (gee_iterator_next (_str_it)) {
			char* str;
			const char* _tmp2;
			str = (char*) gee_iterator_get (_str_it);
			_tmp2 = NULL;
			_vala_array_add2 (&unavail, &unavail_length1, &unavail_size, (_tmp2 = (str), (_tmp2 == NULL) ? NULL : g_strdup (_tmp2)));
			str = (g_free (str), NULL);
		}
		(_str_it == NULL) ? NULL : (_str_it = (g_object_unref (_str_it), NULL));
	}
	g_signal_emit_by_name ((ContextProviderDBusSubscriber*) self, "changed", values, unavail, unavail_length1);
	unavail = (_vala_array_free (unavail, unavail_length1, (GDestroyNotify) g_free), NULL);
}


static void context_provider_subscriber_real_Subscribe (ContextProviderDBusSubscriber* base, char** keys, int keys_length1, GHashTable** values, char*** undeterminable_keys, int* undeterminable_keys_length1) {
	ContextProviderSubscriber * self;
	ContextProviderStringSet* keyset;
	char* _tmp0;
	ContextProviderStringSet* new_keys;
	char* _tmp1;
	ContextProviderStringSet* _tmp2;
	GHashTable* _tmp4;
	GHashTable* _tmp3;
	self = (ContextProviderSubscriber*) base;
	*values = NULL;
	/* Check input against valid keys*/
	keyset = context_provider_manager_check_keys (context_provider_subscriber_manager, keys, keys_length1);
	_tmp0 = NULL;
	g_message ("subscriber.vala:45: Subscribe: requested valid keys %s", _tmp0 = context_provider_string_set_debug (keyset));
	_tmp0 = (g_free (_tmp0), NULL);
	/* Ignore keys which are already subscribed to*/
	new_keys = context_provider_string_set_new_difference (keyset, self->priv->subscribed_keys);
	_tmp1 = NULL;
	g_message ("subscriber.vala:49: Subscribe: new keys %s", _tmp1 = context_provider_string_set_debug (new_keys));
	_tmp1 = (g_free (_tmp1), NULL);
	/* Increase the subscriber count of the new keys.*/
	context_provider_key_usage_counter_add (self->priv->key_counter, new_keys);
	_tmp2 = NULL;
	self->priv->subscribed_keys = (_tmp2 = context_provider_string_set_new_union (self->priv->subscribed_keys, new_keys), (self->priv->subscribed_keys == NULL) ? NULL : (self->priv->subscribed_keys = (context_provider_string_set_unref (self->priv->subscribed_keys), NULL)), _tmp2);
	/* Read the values from the central value table and return them
	 Note: Use also those keys which were already subscribed to (and are subscribed to again)*/
	_tmp4 = NULL;
	_tmp3 = NULL;
	context_provider_manager_get_internal (context_provider_subscriber_manager, keyset, &_tmp3, &(*undeterminable_keys), &(*undeterminable_keys_length1));
	(*values) = (_tmp4 = _tmp3, ((*values) == NULL) ? NULL : ((*values) = (g_hash_table_unref ((*values)), NULL)), _tmp4);
	g_debug ("subscriber.vala:60: Subscribe done");
	(keyset == NULL) ? NULL : (keyset = (context_provider_string_set_unref (keyset), NULL));
	(new_keys == NULL) ? NULL : (new_keys = (context_provider_string_set_unref (new_keys), NULL));
}


static void context_provider_subscriber_real_Unsubscribe (ContextProviderDBusSubscriber* base, char** keys, int keys_length1) {
	ContextProviderSubscriber * self;
	ContextProviderStringSet* keyset;
	ContextProviderStringSet* decreasing_keys;
	ContextProviderStringSet* _tmp0;
	self = (ContextProviderSubscriber*) base;
	/* Check input against valid keys*/
	keyset = context_provider_manager_check_keys (context_provider_subscriber_manager, keys, keys_length1);
	/* Ignore keys which are not subscribed to*/
	decreasing_keys = context_provider_string_set_new_intersection (keyset, self->priv->subscribed_keys);
	/* Decrease the (global) subscriber count for the keys */
	context_provider_key_usage_counter_remove (self->priv->key_counter, decreasing_keys);
	/* Track the keys which are currently subscribed to (with this subscriber) */
	_tmp0 = NULL;
	self->priv->subscribed_keys = (_tmp0 = context_provider_string_set_new_difference (self->priv->subscribed_keys, decreasing_keys), (self->priv->subscribed_keys == NULL) ? NULL : (self->priv->subscribed_keys = (context_provider_string_set_unref (self->priv->subscribed_keys), NULL)), _tmp0);
	(keyset == NULL) ? NULL : (keyset = (context_provider_string_set_unref (keyset), NULL));
	(decreasing_keys == NULL) ? NULL : (decreasing_keys = (context_provider_string_set_unref (decreasing_keys), NULL));
}


static GValue* _g_value_dup (GValue* self) {
	return g_boxed_copy (G_TYPE_VALUE, self);
}


/* FIXME: Other actions needed?
 Is called when the value of a property changes */
void context_provider_subscriber_on_value_changed (ContextProviderSubscriber* self, GHashTable* changed_properties, GeeArrayList* changed_undeterminable) {
	GHashTable* values_to_send;
	GeeArrayList* undeterminable_keys;
	GList* keys;
	gboolean _tmp3;
	g_return_if_fail (self != NULL);
	g_return_if_fail (changed_properties != NULL);
	values_to_send = g_hash_table_new (g_str_hash, g_str_equal);
	undeterminable_keys = gee_array_list_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, g_free, g_direct_equal);
	/* Loop through the properties we got and
	 check if the client is interested in them.*/
	keys = g_hash_table_get_keys (changed_properties);
	/* Note: get_keys returns a list of unowned strings. We shouldn't assign it to
	 a list of owned strings. At the moment, the Vala compiler doesn't prevent us
	 from doing so. */
	{
		GList* key_collection;
		GList* key_it;
		key_collection = keys;
		for (key_it = key_collection; key_it != NULL; key_it = key_it->next) {
			const char* key;
			key = (const char*) key_it->data;
			{
				if (context_provider_string_set_is_member (self->priv->subscribed_keys, key)) {
					GValue* _tmp0 = {0};
					GValue* v;
					/* The client of this subscriber is interested in the key*/
					v = (_tmp0 = (GValue*) g_hash_table_lookup (changed_properties, key), (_tmp0 == NULL) ? NULL : _g_value_dup (_tmp0));
					if (v == NULL) {
						/* FIXME: Is this needed?*/
						gee_collection_add ((GeeCollection*) undeterminable_keys, key);
					} else {
						GValue* _tmp2 = {0};
						const char* _tmp1;
						_tmp1 = NULL;
						g_hash_table_insert (values_to_send, (_tmp1 = key, (_tmp1 == NULL) ? NULL : g_strdup (_tmp1)), (_tmp2 = v, (_tmp2 == NULL) ? NULL : _g_value_dup (_tmp2)));
					}
					(v == NULL) ? NULL : (v = (g_free (v), NULL));
				}
			}
		}
	}
	/* Loop through the undetermined properties we got and
	 check if the client is interested in them.*/
	{
		GeeIterator* _key_it;
		/* Loop through the undetermined properties we got and
		 check if the client is interested in them.*/
		_key_it = gee_iterable_iterator ((GeeIterable*) changed_undeterminable);
		/* Loop through the undetermined properties we got and
		 check if the client is interested in them.*/
		while (gee_iterator_next (_key_it)) {
			char* key;
			/* Loop through the undetermined properties we got and
			 check if the client is interested in them.*/
			key = (char*) gee_iterator_get (_key_it);
			if (context_provider_string_set_is_member (self->priv->subscribed_keys, key)) {
				/* The client of this subscriber is interested in the key*/
				gee_collection_add ((GeeCollection*) undeterminable_keys, key);
			}
			key = (g_free (key), NULL);
		}
		(_key_it == NULL) ? NULL : (_key_it = (g_object_unref (_key_it), NULL));
	}
	_tmp3 = FALSE;
	if (g_hash_table_size (values_to_send) > 0) {
		_tmp3 = TRUE;
	} else {
		_tmp3 = gee_collection_get_size ((GeeCollection*) undeterminable_keys) > 0;
	}
	/* Check if we have something to send to the client*/
	if (_tmp3) {
		context_provider_subscriber_emit_changed (self, values_to_send, undeterminable_keys);
	}
	(values_to_send == NULL) ? NULL : (values_to_send = (g_hash_table_unref (values_to_send), NULL));
	(undeterminable_keys == NULL) ? NULL : (undeterminable_keys = (g_object_unref (undeterminable_keys), NULL));
	(keys == NULL) ? NULL : (keys = (g_list_free (keys), NULL));
}


const char* context_provider_subscriber_get_object_path (ContextProviderSubscriber* self) {
	g_return_val_if_fail (self != NULL, NULL);
	return self->priv->_object_path;
}


void context_provider_subscriber_set_object_path (ContextProviderSubscriber* self, const char* value) {
	char* _tmp2;
	const char* _tmp1;
	g_return_if_fail (self != NULL);
	_tmp2 = NULL;
	_tmp1 = NULL;
	self->priv->_object_path = (_tmp2 = (_tmp1 = value, (_tmp1 == NULL) ? NULL : g_strdup (_tmp1)), self->priv->_object_path = (g_free (self->priv->_object_path), NULL), _tmp2);
	g_object_notify ((GObject *) self, "object-path");
}


static void context_provider_subscriber_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec) {
	ContextProviderSubscriber * self;
	gpointer boxed;
	self = CONTEXT_PROVIDER_SUBSCRIBER (object);
	switch (property_id) {
		case CONTEXT_PROVIDER_SUBSCRIBER_OBJECT_PATH:
		g_value_set_string (value, context_provider_subscriber_get_object_path (self));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void context_provider_subscriber_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec) {
	ContextProviderSubscriber * self;
	self = CONTEXT_PROVIDER_SUBSCRIBER (object);
	switch (property_id) {
		case CONTEXT_PROVIDER_SUBSCRIBER_OBJECT_PATH:
		context_provider_subscriber_set_object_path (self, g_value_get_string (value));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


void _context_provider_subscriber_dbus_unregister (DBusConnection* connection, void* user_data) {
}


static DBusMessage* _dbus_context_provider_subscriber_introspect (ContextProviderSubscriber* self, DBusConnection* connection, DBusMessage* message) {
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


DBusHandlerResult context_provider_subscriber_dbus_message (DBusConnection* connection, DBusMessage* message, void* object) {
	DBusMessage* reply;
	reply = NULL;
	if (dbus_message_is_method_call (message, "org.freedesktop.DBus.Introspectable", "Introspect")) {
		reply = _dbus_context_provider_subscriber_introspect (object, connection, message);
	}
	if (reply) {
		dbus_connection_send (connection, reply, NULL);
		dbus_message_unref (reply);
		return DBUS_HANDLER_RESULT_HANDLED;
	} else if (context_provider_dbus_subscriber_dbus_message (connection, message, object) == DBUS_HANDLER_RESULT_HANDLED) {
		return DBUS_HANDLER_RESULT_HANDLED;
	} else {
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}
}


void context_provider_subscriber_dbus_register_object (DBusConnection* connection, const char* path, void* object) {
	if (!g_object_get_data (object, "dbus_object_path")) {
		g_object_set_data (object, "dbus_object_path", g_strdup (path));
		dbus_connection_register_object_path (connection, path, &_context_provider_subscriber_dbus_path_vtable, object);
	}
	context_provider_dbus_subscriber_dbus_register_object (connection, path, object);
}


static void context_provider_subscriber_class_init (ContextProviderSubscriberClass * klass) {
	context_provider_subscriber_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (ContextProviderSubscriberPrivate));
	G_OBJECT_CLASS (klass)->get_property = context_provider_subscriber_get_property;
	G_OBJECT_CLASS (klass)->set_property = context_provider_subscriber_set_property;
	G_OBJECT_CLASS (klass)->finalize = context_provider_subscriber_finalize;
	g_object_class_install_property (G_OBJECT_CLASS (klass), CONTEXT_PROVIDER_SUBSCRIBER_OBJECT_PATH, g_param_spec_pointer ("object-path", "object-path", "object-path", G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_type_set_qdata (CONTEXT_PROVIDER_TYPE_SUBSCRIBER, g_quark_from_static_string ("DBusObjectVTable"), (void*) (&_context_provider_subscriber_dbus_vtable));
}


static void context_provider_subscriber_context_provider_dbus_subscriber_interface_init (ContextProviderDBusSubscriberIface * iface) {
	context_provider_subscriber_context_provider_dbus_subscriber_parent_iface = g_type_interface_peek_parent (iface);
	iface->Subscribe = context_provider_subscriber_real_Subscribe;
	iface->Unsubscribe = context_provider_subscriber_real_Unsubscribe;
}


static void context_provider_subscriber_instance_init (ContextProviderSubscriber * self) {
	self->priv = CONTEXT_PROVIDER_SUBSCRIBER_GET_PRIVATE (self);
}


static void context_provider_subscriber_finalize (GObject* obj) {
	ContextProviderSubscriber * self;
	self = CONTEXT_PROVIDER_SUBSCRIBER (obj);
	{
		g_debug ("subscriber.vala:23: Subscriber %s destroyed", self->priv->_object_path);
		/* TODO: Unsubscribe all the keys currently subscribed to
		 Decrease the (global) subscriber count for the keys*/
		context_provider_key_usage_counter_remove (self->priv->key_counter, self->priv->subscribed_keys);
	}
	self->priv->_object_path = (g_free (self->priv->_object_path), NULL);
	(self->priv->subscribed_keys == NULL) ? NULL : (self->priv->subscribed_keys = (context_provider_string_set_unref (self->priv->subscribed_keys), NULL));
	(self->priv->key_counter == NULL) ? NULL : (self->priv->key_counter = (context_provider_key_usage_counter_unref (self->priv->key_counter), NULL));
	G_OBJECT_CLASS (context_provider_subscriber_parent_class)->finalize (obj);
}


GType context_provider_subscriber_get_type (void) {
	static GType context_provider_subscriber_type_id = 0;
	if (context_provider_subscriber_type_id == 0) {
		static const GTypeInfo g_define_type_info = { sizeof (ContextProviderSubscriberClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) context_provider_subscriber_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ContextProviderSubscriber), 0, (GInstanceInitFunc) context_provider_subscriber_instance_init, NULL };
		static const GInterfaceInfo context_provider_dbus_subscriber_info = { (GInterfaceInitFunc) context_provider_subscriber_context_provider_dbus_subscriber_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		context_provider_subscriber_type_id = g_type_register_static (G_TYPE_OBJECT, "ContextProviderSubscriber", &g_define_type_info, 0);
		g_type_add_interface_static (context_provider_subscriber_type_id, CONTEXT_PROVIDER_TYPE_DBUS_SUBSCRIBER, &context_provider_dbus_subscriber_info);
	}
	return context_provider_subscriber_type_id;
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


static void _vala_dbus_register_object (DBusConnection* connection, const char* path, void* object) {
	const _DBusObjectVTable * vtable;
	vtable = g_type_get_qdata (G_TYPE_FROM_INSTANCE (object), g_quark_from_static_string ("DBusObjectVTable"));
	if (vtable) {
		vtable->register_object (connection, path, object);
	} else {
		g_warning ("Object does not implement any D-Bus interface");
	}
}




