
#include <manager.h>
#include <gee/hashmap.h>
#include <gee/iterable.h>
#include <gee/iterator.h>
#include <gee/map.h>
#include <subscriber.h>

typedef struct _DBusObjectVTable _DBusObjectVTable;

struct _DBusObjectVTable {
	void (*register_object) (DBusConnection*, const char*, void*);
};



struct _ContextProviderManagerPrivate {
	GeeHashMap* subscribers;
	ContextProviderProviders* _providers;
	ContextProviderKeyUsageCounter* _key_counter;
	GHashTable* values;
	gint subscriber_count;
};

#define CONTEXT_PROVIDER_MANAGER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), CONTEXT_PROVIDER_TYPE_MANAGER, ContextProviderManagerPrivate))
enum  {
	CONTEXT_PROVIDER_MANAGER_DUMMY_PROPERTY,
	CONTEXT_PROVIDER_MANAGER_PROVIDERS,
	CONTEXT_PROVIDER_MANAGER_KEY_COUNTER
};
static DBusGProxy* context_provider_manager_bus = NULL;
static DBusBusType context_provider_manager_busType = DBUS_BUS_SESSION;
static ContextProviderManager* context_provider_manager_instance = NULL;
static void _context_provider_manager_on_name_owner_changed_dynamic_NameOwnerChanged0_ (DBusGProxy* _sender, const char* name, const char* old_owner, const char* new_owner, gpointer self);
void _dynamic_NameOwnerChanged1_connect (gpointer obj, const char * signal_name, GCallback handler, gpointer data);
static ContextProviderManager* context_provider_manager_construct (GType object_type);
static ContextProviderManager* context_provider_manager_new (void);
static void context_provider_manager_real_Get (ContextProviderDBusManager* base, char** keys, int keys_length1, GHashTable** values_to_send, char*** undeterminable_keys, int* undeterminable_keys_length1);
static char* context_provider_manager_real_GetSubscriber (ContextProviderDBusManager* base, const char* name, GError** error);
static void context_provider_manager_on_name_owner_changed (ContextProviderManager* self, DBusGProxy* sender, const char* name, const char* old_owner, const char* new_owner);
static GValue* _g_value_dup (GValue* self);
static void context_provider_manager_insert_to_value_table (ContextProviderManager* self, GHashTable* properties, GeeArrayList* undeterminable_keys);
static void _vala_array_add1 (char*** array, int* length, int* size, char* value);
static char** _vala_array_dup1 (char** self, int length);
static void context_provider_manager_read_from_value_table (ContextProviderManager* self, ContextProviderStringSet* keys, GHashTable** properties, char*** undeterminable_keys, int* undeterminable_keys_length1);
static void context_provider_manager_set_providers (ContextProviderManager* self, ContextProviderProviders* value);
static void context_provider_manager_set_key_counter (ContextProviderManager* self, ContextProviderKeyUsageCounter* value);
static gpointer context_provider_manager_parent_class = NULL;
void _context_provider_manager_dbus_unregister (DBusConnection* connection, void* user_data);
static DBusMessage* _dbus_context_provider_manager_introspect (ContextProviderManager* self, DBusConnection* connection, DBusMessage* message);
static ContextProviderDBusManagerIface* context_provider_manager_context_provider_dbus_manager_parent_iface = NULL;
static void context_provider_manager_finalize (GObject* obj);
static void _vala_array_free (gpointer array, gint array_length, GDestroyNotify destroy_func);
static int _vala_strcmp0 (const char * str1, const char * str2);
static void _vala_dbus_register_object (DBusConnection* connection, const char* path, void* object);

static const DBusObjectPathVTable _context_provider_manager_dbus_path_vtable = {_context_provider_manager_dbus_unregister, context_provider_manager_dbus_message};
static const _DBusObjectVTable _context_provider_manager_dbus_vtable = {context_provider_manager_dbus_register_object};

static void g_cclosure_user_marshal_VOID__STRING_STRING_STRING (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);

ContextProviderManager* context_provider_manager_get_instance (void) {
	ContextProviderManager* _tmp1;
	if (context_provider_manager_instance == NULL) {
		ContextProviderManager* _tmp0;
		_tmp0 = NULL;
		context_provider_manager_instance = (_tmp0 = context_provider_manager_new (), (context_provider_manager_instance == NULL) ? NULL : (context_provider_manager_instance = (g_object_unref (context_provider_manager_instance), NULL)), _tmp0);
	}
	_tmp1 = NULL;
	return (_tmp1 = context_provider_manager_instance, (_tmp1 == NULL) ? NULL : g_object_ref (_tmp1));
}


static void _context_provider_manager_on_name_owner_changed_dynamic_NameOwnerChanged0_ (DBusGProxy* _sender, const char* name, const char* old_owner, const char* new_owner, gpointer self) {
	context_provider_manager_on_name_owner_changed (self, _sender, name, old_owner, new_owner);
}


void _dynamic_NameOwnerChanged1_connect (gpointer obj, const char * signal_name, GCallback handler, gpointer data) {
	dbus_g_object_register_marshaller (g_cclosure_user_marshal_VOID__STRING_STRING_STRING, G_TYPE_NONE, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INVALID);
	dbus_g_proxy_add_signal (obj, "NameOwnerChanged", G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (obj, signal_name, handler, data, NULL);
}


static ContextProviderManager* context_provider_manager_construct (GType object_type) {
	GError * inner_error;
	ContextProviderManager * self;
	ContextProviderProviders* _tmp0;
	ContextProviderKeyUsageCounter* _tmp1;
	GeeHashMap* _tmp2;
	GHashTable* _tmp3;
	DBusGConnection* connection;
	DBusGProxy* _tmp4;
	inner_error = NULL;
	self = g_object_newv (object_type, 0, NULL);
	_tmp0 = NULL;
	context_provider_manager_set_providers (self, _tmp0 = context_provider_providers_new ());
	(_tmp0 == NULL) ? NULL : (_tmp0 = (context_provider_providers_unref (_tmp0), NULL));
	_tmp1 = NULL;
	context_provider_manager_set_key_counter (self, _tmp1 = context_provider_key_usage_counter_new ());
	(_tmp1 == NULL) ? NULL : (_tmp1 = (context_provider_key_usage_counter_unref (_tmp1), NULL));
	_tmp2 = NULL;
	self->priv->subscribers = (_tmp2 = gee_hash_map_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, g_free, CONTEXT_PROVIDER_TYPE_SUBSCRIBER, (GBoxedCopyFunc) g_object_ref, g_object_unref, g_str_hash, g_str_equal, g_direct_equal), (self->priv->subscribers == NULL) ? NULL : (self->priv->subscribers = (g_object_unref (self->priv->subscribers), NULL)), _tmp2);
	_tmp3 = NULL;
	self->priv->values = (_tmp3 = g_hash_table_new (g_str_hash, g_str_equal), (self->priv->values == NULL) ? NULL : (self->priv->values = (g_hash_table_unref (self->priv->values), NULL)), _tmp3);
	connection = dbus_g_bus_get (context_provider_manager_busType, &inner_error);
	if (inner_error != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, inner_error->message);
		g_clear_error (&inner_error);
		return;
	}
	_tmp4 = NULL;
	context_provider_manager_bus = (_tmp4 = dbus_g_proxy_new_for_name (connection, "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus"), (context_provider_manager_bus == NULL) ? NULL : (context_provider_manager_bus = (g_object_unref (context_provider_manager_bus), NULL)), _tmp4);
	_dynamic_NameOwnerChanged1_connect (context_provider_manager_bus, "NameOwnerChanged", (GCallback) _context_provider_manager_on_name_owner_changed_dynamic_NameOwnerChanged0_, self);
	return self;
}


static ContextProviderManager* context_provider_manager_new (void) {
	return context_provider_manager_construct (CONTEXT_PROVIDER_TYPE_MANAGER);
}


void context_provider_manager_set_bus_type (DBusBusType b) {
	context_provider_manager_busType = b;
}


static void context_provider_manager_real_Get (ContextProviderDBusManager* base, char** keys, int keys_length1, GHashTable** values_to_send, char*** undeterminable_keys, int* undeterminable_keys_length1) {
	ContextProviderManager * self;
	ContextProviderStringSet* keyset;
	GHashTable* _tmp1;
	GHashTable* _tmp0;
	self = (ContextProviderManager*) base;
	*values_to_send = NULL;
	g_debug ("manager.vala:49: Manager.Get");
	/* Check input against valid keys*/
	keyset = context_provider_manager_check_keys (self, keys, keys_length1);
	_tmp1 = NULL;
	_tmp0 = NULL;
	context_provider_manager_get_internal (self, keyset, &_tmp0, &(*undeterminable_keys), &(*undeterminable_keys_length1));
	(*values_to_send) = (_tmp1 = _tmp0, ((*values_to_send) == NULL) ? NULL : ((*values_to_send) = (g_hash_table_unref ((*values_to_send)), NULL)), _tmp1);
	(keyset == NULL) ? NULL : (keyset = (context_provider_string_set_unref (keyset), NULL));
}


void context_provider_manager_get_internal (ContextProviderManager* self, ContextProviderStringSet* keyset, GHashTable** values_to_send, char*** undeterminable_keys, int* undeterminable_keys_length1) {
	GHashTable* values;
	GeeArrayList* undeterminable_key_list;
	GHashTable* _tmp1;
	GHashTable* _tmp0;
	g_return_if_fail (self != NULL);
	g_return_if_fail (keyset != NULL);
	*values_to_send = NULL;
	values = g_hash_table_new (g_str_hash, g_str_equal);
	/* Let providers update the central value table*/
	undeterminable_key_list = context_provider_providers_get (self->priv->_providers, keyset, values);
	context_provider_manager_insert_to_value_table (self, values, undeterminable_key_list);
	/* Then read the values from the value table*/
	_tmp1 = NULL;
	_tmp0 = NULL;
	context_provider_manager_read_from_value_table (self, keyset, &_tmp0, &(*undeterminable_keys), &(*undeterminable_keys_length1));
	(*values_to_send) = (_tmp1 = _tmp0, ((*values_to_send) == NULL) ? NULL : ((*values_to_send) = (g_hash_table_unref ((*values_to_send)), NULL)), _tmp1);
	(values == NULL) ? NULL : (values = (g_hash_table_unref (values), NULL));
	(undeterminable_key_list == NULL) ? NULL : (undeterminable_key_list = (g_object_unref (undeterminable_key_list), NULL));
}


static char* context_provider_manager_real_GetSubscriber (ContextProviderDBusManager* base, const char* name, GError** error) {
	ContextProviderManager * self;
	GError * inner_error;
	DBusGConnection* connection;
	ContextProviderSubscriber* s;
	const char* _tmp2;
	char* _tmp3;
	self = (ContextProviderManager*) base;
	g_return_val_if_fail (name != NULL, NULL);
	inner_error = NULL;
	/* First check if the same client has already requested a subscriber object.
	If so, return its object path. */
	if (gee_map_contains ((GeeMap*) self->priv->subscribers, name)) {
		ContextProviderSubscriber* s;
		const char* _tmp0;
		char* _tmp1;
		s = (ContextProviderSubscriber*) gee_map_get ((GeeMap*) self->priv->subscribers, name);
		_tmp0 = NULL;
		_tmp1 = NULL;
		return (_tmp1 = (_tmp0 = context_provider_subscriber_get_object_path (s), (_tmp0 == NULL) ? NULL : g_strdup (_tmp0)), (s == NULL) ? NULL : (s = (g_object_unref (s), NULL)), _tmp1);
	}
	/* Else, create a new subscriber and return its object path.
	
	 Note: Use session / system bus according to the configuration
	 (which can be changed via setBusType).*/
	connection = dbus_g_bus_get (context_provider_manager_busType, &inner_error);
	if (inner_error != NULL) {
		g_propagate_error (error, inner_error);
		return NULL;
	}
	/* Create a subscription object*/
	s = context_provider_subscriber_new (self, self->priv->_key_counter, self->priv->subscriber_count);
	self->priv->subscriber_count++;
	/*keep a count rather than use subscribers.length for immutability
	 FIXME: Potential overflow? Do we need to worry?
	 Store information about this newly created subscription object*/
	gee_map_set ((GeeMap*) self->priv->subscribers, name, s);
	/* Track the dbus address
	 Return the object path of the subscription object to the client*/
	_vala_dbus_register_object (dbus_g_connection_get_connection (connection), (const char*) context_provider_subscriber_get_object_path (s), (GObject*) s);
	_tmp2 = NULL;
	_tmp3 = NULL;
	return (_tmp3 = (_tmp2 = context_provider_subscriber_get_object_path (s), (_tmp2 == NULL) ? NULL : g_strdup (_tmp2)), (connection == NULL) ? NULL : (connection = (dbus_g_connection_unref (connection), NULL)), (s == NULL) ? NULL : (s = (g_object_unref (s), NULL)), _tmp3);
}


/*
Listen to subscribers exiting.
*/
static void context_provider_manager_on_name_owner_changed (ContextProviderManager* self, DBusGProxy* sender, const char* name, const char* old_owner, const char* new_owner) {
	gboolean _tmp0;
	g_return_if_fail (self != NULL);
	g_return_if_fail (sender != NULL);
	g_return_if_fail (name != NULL);
	g_return_if_fail (old_owner != NULL);
	g_return_if_fail (new_owner != NULL);
	g_debug ("manager.vala:101: Got a NameOwnerChanged signal");
	g_debug (name);
	g_debug (old_owner);
	g_debug (new_owner);
	_tmp0 = FALSE;
	if (gee_map_contains ((GeeMap*) self->priv->subscribers, name)) {
		_tmp0 = _vala_strcmp0 (new_owner, "") == 0;
	} else {
		_tmp0 = FALSE;
	}
	if (_tmp0) {
		g_debug ("manager.vala:107: Client died");
		/* Remove the subscriber*/
		gee_map_remove ((GeeMap*) self->priv->subscribers, name);
	}
}


/* Now nobody holds a pointer to the subscriber so it is destroyed automatically.

Checks which keys are valid and returns them.
*/
ContextProviderStringSet* context_provider_manager_check_keys (ContextProviderManager* self, char** keys, int keys_length1) {
	ContextProviderStringSet* checked_keys;
	g_return_val_if_fail (self != NULL, NULL);
	/* Do not create a StringSet from the parameter "keys" as that would be add Quarks*/
	checked_keys = context_provider_string_set_new ();
	{
		char** key_collection;
		int key_collection_length1;
		int key_it;
		key_collection = keys;
		key_collection_length1 = keys_length1;
		for (key_it = 0; key_it < keys_length1; key_it = key_it + 1) {
			const char* _tmp0;
			char* key;
			_tmp0 = NULL;
			key = (_tmp0 = key_collection[key_it], (_tmp0 == NULL) ? NULL : g_strdup (_tmp0));
			{
				if (context_provider_string_set_is_member (context_provider_providers_get_valid_keys (self->priv->_providers), key)) {
					context_provider_string_set_add (checked_keys, key);
				}
				key = (g_free (key), NULL);
			}
		}
	}
	return checked_keys;
}


static GValue* _g_value_dup (GValue* self) {
	return g_boxed_copy (G_TYPE_VALUE, self);
}


/*
Update the value table with new values.
*/
static void context_provider_manager_insert_to_value_table (ContextProviderManager* self, GHashTable* properties, GeeArrayList* undeterminable_keys) {
	GList* keys;
	g_return_if_fail (self != NULL);
	g_return_if_fail (properties != NULL);
	/*debug ("insert_to_value_table");*/
	keys = g_hash_table_get_keys (properties);
	/* Note: get_keys returns a list of unowned strings. We shouldn't assign it to
	 a list of owned strings. At the moment, the Vala compiler doesn't prevent us
	 from doing so.*/
	{
		GList* key_collection;
		GList* key_it;
		key_collection = keys;
		for (key_it = key_collection; key_it != NULL; key_it = key_it->next) {
			const char* key;
			key = (const char*) key_it->data;
			{
				GValue* _tmp1 = {0};
				const char* _tmp0;
				/* Overwrite the value in the value table.
				 Do not care whether it was already there or not.*/
				_tmp0 = NULL;
				g_hash_table_insert (self->priv->values, (_tmp0 = key, (_tmp0 == NULL) ? NULL : g_strdup (_tmp0)), (_tmp1 = (GValue*) g_hash_table_lookup (properties, key), (_tmp1 == NULL) ? NULL : _g_value_dup (_tmp1)));
			}
		}
	}
	{
		GeeIterator* _key_it;
		_key_it = gee_iterable_iterator ((GeeIterable*) undeterminable_keys);
		while (gee_iterator_next (_key_it)) {
			char* key;
			const char* _tmp2;
			key = (char*) gee_iterator_get (_key_it);
			_tmp2 = NULL;
			g_hash_table_insert (self->priv->values, (_tmp2 = key, (_tmp2 == NULL) ? NULL : g_strdup (_tmp2)), NULL);
			key = (g_free (key), NULL);
		}
		(_key_it == NULL) ? NULL : (_key_it = (g_object_unref (_key_it), NULL));
	}
	(keys == NULL) ? NULL : (keys = (g_list_free (keys), NULL));
}


static void _vala_array_add1 (char*** array, int* length, int* size, char* value) {
	if ((*length) == (*size)) {
		*size = (*size) ? (2 * (*size)) : 4;
		*array = g_renew (char*, *array, (*size) + 1);
	}
	(*array)[(*length)++] = value;
	(*array)[*length] = NULL;
}


static char** _vala_array_dup1 (char** self, int length) {
	char** result;
	int i;
	const char* _tmp8;
	result = g_new0 (char*, length);
	for (i = 0; i < length; i++) {
		result[i] = (_tmp8 = self[i], (_tmp8 == NULL) ? NULL : g_strdup (_tmp8));
	}
	return result;
}


/*
Read the values of the specified keys from the value table.
*/
static void context_provider_manager_read_from_value_table (ContextProviderManager* self, ContextProviderStringSet* keys, GHashTable** properties, char*** undeterminable_keys, int* undeterminable_keys_length1) {
	GHashTable* _tmp0;
	char** _tmp2;
	gint undeterminable_keys_temp_size;
	gint undeterminable_keys_temp_length1;
	char** _tmp1;
	char** undeterminable_keys_temp;
	char** _tmp10;
	char** _tmp9;
	const char* _tmp8;
	g_return_if_fail (self != NULL);
	g_return_if_fail (keys != NULL);
	*properties = NULL;
	g_debug ("manager.vala:153: read_from_value_table");
	/* Note: Vala doesn't support += for parameters yet; using a temp array*/
	_tmp0 = NULL;
	(*properties) = (_tmp0 = g_hash_table_new (g_str_hash, g_str_equal), ((*properties) == NULL) ? NULL : ((*properties) = (g_hash_table_unref ((*properties)), NULL)), _tmp0);
	_tmp2 = NULL;
	_tmp1 = NULL;
	undeterminable_keys_temp = (_tmp2 = (_tmp1 = g_new0 (char*, 0 + 1), _tmp1), undeterminable_keys_temp_length1 = 0, undeterminable_keys_temp_size = undeterminable_keys_temp_length1, _tmp2);
	{
		ContextProviderStringSetIter* _key_it;
		_key_it = context_provider_string_set_iterator (keys);
		while (context_provider_string_set_iter_next (_key_it)) {
			char* key;
			GValue* _tmp3 = {0};
			GValue* v;
			key = context_provider_string_set_iter_get (_key_it);
			g_debug ("manager.vala:158:   %s", key);
			/*debug ("reading value for key %s", key);*/
			v = (_tmp3 = (GValue*) g_hash_table_lookup (self->priv->values, key), (_tmp3 == NULL) ? NULL : _g_value_dup (_tmp3));
			if (v == NULL) {
				const char* _tmp4;
				g_debug ("manager.vala:163:     - undeterminable");
				_tmp4 = NULL;
				_vala_array_add1 (&undeterminable_keys_temp, &undeterminable_keys_temp_length1, &undeterminable_keys_temp_size, (_tmp4 = (key), (_tmp4 == NULL) ? NULL : g_strdup (_tmp4)));
			} else {
				char* _tmp5;
				GValue* _tmp7 = {0};
				const char* _tmp6;
				_tmp5 = NULL;
				g_debug ("manager.vala:167:     - %s", _tmp5 = g_strdup_value_contents (v));
				_tmp5 = (g_free (_tmp5), NULL);
				_tmp6 = NULL;
				g_hash_table_insert ((*properties), (_tmp6 = key, (_tmp6 == NULL) ? NULL : g_strdup (_tmp6)), (_tmp7 = v, (_tmp7 == NULL) ? NULL : _g_value_dup (_tmp7)));
			}
			key = (g_free (key), NULL);
			(v == NULL) ? NULL : (v = (g_free (v), NULL));
		}
		(_key_it == NULL) ? NULL : (_key_it = (context_provider_string_set_iter_unref (_key_it), NULL));
	}
	_tmp10 = NULL;
	_tmp9 = NULL;
	_tmp8 = NULL;
	(*undeterminable_keys) = (_tmp10 = (_tmp9 = undeterminable_keys_temp, (_tmp9 == NULL) ? ((gpointer) _tmp9) : _vala_array_dup1 (_tmp9, undeterminable_keys_temp_length1)), (*undeterminable_keys) = (_vala_array_free ((*undeterminable_keys), *undeterminable_keys_length1, (GDestroyNotify) g_free), NULL), *undeterminable_keys_length1 = undeterminable_keys_temp_length1, _tmp10);
	g_debug ("manager.vala:172: read_from_value_table done");
	undeterminable_keys_temp = (_vala_array_free (undeterminable_keys_temp, undeterminable_keys_temp_length1, (GDestroyNotify) g_free), NULL);
}


/* Is called when the provider sets new values to context properties */
gboolean context_provider_manager_property_values_changed (ContextProviderManager* self, GHashTable* properties, GeeArrayList* undeterminable_keys) {
	GList* keys;
	gboolean _tmp4;
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (properties != NULL, FALSE);
	/* Check that all the keys are valid*/
	keys = g_hash_table_get_keys (properties);
	{
		GList* key_collection;
		GList* key_it;
		key_collection = keys;
		for (key_it = key_collection; key_it != NULL; key_it = key_it->next) {
			const char* key;
			key = (const char*) key_it->data;
			{
				if (context_provider_string_set_is_member (context_provider_providers_get_valid_keys (self->priv->_providers), key) == FALSE) {
					gboolean _tmp0;
					g_debug ("manager.vala:181: Key %s is not valid", key);
					g_assert (FALSE);
					/* FIXME: How to react?
					 Now we drop the whole event and return an error value*/
					return (_tmp0 = FALSE, (keys == NULL) ? NULL : (keys = (g_list_free (keys), NULL)), _tmp0);
				}
			}
		}
	}
	{
		GeeIterator* _key_it;
		_key_it = gee_iterable_iterator ((GeeIterable*) undeterminable_keys);
		while (gee_iterator_next (_key_it)) {
			char* key;
			key = (char*) gee_iterator_get (_key_it);
			if (context_provider_string_set_is_member (context_provider_providers_get_valid_keys (self->priv->_providers), key) == FALSE) {
				gboolean _tmp1;
				g_debug ("manager.vala:190: Key %s is not valid", key);
				g_assert (FALSE);
				/* FIXME: How to react?
				 Now we drop the whole event and return an error value*/
				return (_tmp1 = FALSE, key = (g_free (key), NULL), (_key_it == NULL) ? NULL : (_key_it = (g_object_unref (_key_it), NULL)), (keys == NULL) ? NULL : (keys = (g_list_free (keys), NULL)), _tmp1);
			}
			key = (g_free (key), NULL);
		}
		(_key_it == NULL) ? NULL : (_key_it = (g_object_unref (_key_it), NULL));
	}
	/* Update the value table*/
	context_provider_manager_insert_to_value_table (self, properties, undeterminable_keys);
	/* Inform the subscribers of the change*/
	{
		GeeCollection* _tmp2;
		GeeIterator* _tmp3;
		GeeIterator* _s_it;
		/* Inform the subscribers of the change*/
		_tmp2 = NULL;
		_tmp3 = NULL;
		_s_it = (_tmp3 = gee_iterable_iterator ((GeeIterable*) (_tmp2 = gee_map_get_values ((GeeMap*) self->priv->subscribers))), (_tmp2 == NULL) ? NULL : (_tmp2 = (g_object_unref (_tmp2), NULL)), _tmp3);
		/* Inform the subscribers of the change*/
		while (gee_iterator_next (_s_it)) {
			ContextProviderSubscriber* s;
			/* Inform the subscribers of the change*/
			s = (ContextProviderSubscriber*) gee_iterator_get (_s_it);
			context_provider_subscriber_on_value_changed (s, properties, undeterminable_keys);
			(s == NULL) ? NULL : (s = (g_object_unref (s), NULL));
		}
		(_s_it == NULL) ? NULL : (_s_it = (g_object_unref (_s_it), NULL));
	}
	return (_tmp4 = TRUE, (keys == NULL) ? NULL : (keys = (g_list_free (keys), NULL)), _tmp4);
}


ContextProviderProviders* context_provider_manager_get_providers (ContextProviderManager* self) {
	g_return_val_if_fail (self != NULL, NULL);
	return self->priv->_providers;
}


static void context_provider_manager_set_providers (ContextProviderManager* self, ContextProviderProviders* value) {
	ContextProviderProviders* _tmp2;
	ContextProviderProviders* _tmp1;
	g_return_if_fail (self != NULL);
	_tmp2 = NULL;
	_tmp1 = NULL;
	self->priv->_providers = (_tmp2 = (_tmp1 = value, (_tmp1 == NULL) ? NULL : context_provider_providers_ref (_tmp1)), (self->priv->_providers == NULL) ? NULL : (self->priv->_providers = (context_provider_providers_unref (self->priv->_providers), NULL)), _tmp2);
	g_object_notify ((GObject *) self, "providers");
}


ContextProviderKeyUsageCounter* context_provider_manager_get_key_counter (ContextProviderManager* self) {
	g_return_val_if_fail (self != NULL, NULL);
	return self->priv->_key_counter;
}


static void context_provider_manager_set_key_counter (ContextProviderManager* self, ContextProviderKeyUsageCounter* value) {
	ContextProviderKeyUsageCounter* _tmp2;
	ContextProviderKeyUsageCounter* _tmp1;
	g_return_if_fail (self != NULL);
	_tmp2 = NULL;
	_tmp1 = NULL;
	self->priv->_key_counter = (_tmp2 = (_tmp1 = value, (_tmp1 == NULL) ? NULL : context_provider_key_usage_counter_ref (_tmp1)), (self->priv->_key_counter == NULL) ? NULL : (self->priv->_key_counter = (context_provider_key_usage_counter_unref (self->priv->_key_counter), NULL)), _tmp2);
	g_object_notify ((GObject *) self, "key-counter");
}


static void context_provider_manager_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec) {
	ContextProviderManager * self;
	gpointer boxed;
	self = CONTEXT_PROVIDER_MANAGER (object);
	switch (property_id) {
		case CONTEXT_PROVIDER_MANAGER_PROVIDERS:
		context_provider_value_set_providers (value, context_provider_manager_get_providers (self));
		break;
		case CONTEXT_PROVIDER_MANAGER_KEY_COUNTER:
		context_provider_value_set_key_usage_counter (value, context_provider_manager_get_key_counter (self));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void context_provider_manager_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec) {
	ContextProviderManager * self;
	self = CONTEXT_PROVIDER_MANAGER (object);
	switch (property_id) {
		case CONTEXT_PROVIDER_MANAGER_PROVIDERS:
		context_provider_manager_set_providers (self, context_provider_value_get_providers (value));
		break;
		case CONTEXT_PROVIDER_MANAGER_KEY_COUNTER:
		context_provider_manager_set_key_counter (self, context_provider_value_get_key_usage_counter (value));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


void _context_provider_manager_dbus_unregister (DBusConnection* connection, void* user_data) {
}


static DBusMessage* _dbus_context_provider_manager_introspect (ContextProviderManager* self, DBusConnection* connection, DBusMessage* message) {
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


DBusHandlerResult context_provider_manager_dbus_message (DBusConnection* connection, DBusMessage* message, void* object) {
	DBusMessage* reply;
	reply = NULL;
	if (dbus_message_is_method_call (message, "org.freedesktop.DBus.Introspectable", "Introspect")) {
		reply = _dbus_context_provider_manager_introspect (object, connection, message);
	}
	if (reply) {
		dbus_connection_send (connection, reply, NULL);
		dbus_message_unref (reply);
		return DBUS_HANDLER_RESULT_HANDLED;
	} else if (context_provider_dbus_manager_dbus_message (connection, message, object) == DBUS_HANDLER_RESULT_HANDLED) {
		return DBUS_HANDLER_RESULT_HANDLED;
	} else {
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}
}


void context_provider_manager_dbus_register_object (DBusConnection* connection, const char* path, void* object) {
	if (!g_object_get_data (object, "dbus_object_path")) {
		g_object_set_data (object, "dbus_object_path", g_strdup (path));
		dbus_connection_register_object_path (connection, path, &_context_provider_manager_dbus_path_vtable, object);
	}
	context_provider_dbus_manager_dbus_register_object (connection, path, object);
}


static void context_provider_manager_class_init (ContextProviderManagerClass * klass) {
	context_provider_manager_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (ContextProviderManagerPrivate));
	G_OBJECT_CLASS (klass)->get_property = context_provider_manager_get_property;
	G_OBJECT_CLASS (klass)->set_property = context_provider_manager_set_property;
	G_OBJECT_CLASS (klass)->finalize = context_provider_manager_finalize;
	g_object_class_install_property (G_OBJECT_CLASS (klass), CONTEXT_PROVIDER_MANAGER_PROVIDERS, g_param_spec_pointer ("providers", "providers", "providers", G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), CONTEXT_PROVIDER_MANAGER_KEY_COUNTER, g_param_spec_pointer ("key-counter", "key-counter", "key-counter", G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_type_set_qdata (CONTEXT_PROVIDER_TYPE_MANAGER, g_quark_from_static_string ("DBusObjectVTable"), (void*) (&_context_provider_manager_dbus_vtable));
}


static void context_provider_manager_context_provider_dbus_manager_interface_init (ContextProviderDBusManagerIface * iface) {
	context_provider_manager_context_provider_dbus_manager_parent_iface = g_type_interface_peek_parent (iface);
	iface->Get = context_provider_manager_real_Get;
	iface->GetSubscriber = context_provider_manager_real_GetSubscriber;
}


static void context_provider_manager_instance_init (ContextProviderManager * self) {
	self->priv = CONTEXT_PROVIDER_MANAGER_GET_PRIVATE (self);
	self->priv->subscriber_count = 0;
}


static void context_provider_manager_finalize (GObject* obj) {
	ContextProviderManager * self;
	self = CONTEXT_PROVIDER_MANAGER (obj);
	(self->priv->subscribers == NULL) ? NULL : (self->priv->subscribers = (g_object_unref (self->priv->subscribers), NULL));
	(self->priv->_providers == NULL) ? NULL : (self->priv->_providers = (context_provider_providers_unref (self->priv->_providers), NULL));
	(self->priv->_key_counter == NULL) ? NULL : (self->priv->_key_counter = (context_provider_key_usage_counter_unref (self->priv->_key_counter), NULL));
	(self->priv->values == NULL) ? NULL : (self->priv->values = (g_hash_table_unref (self->priv->values), NULL));
	G_OBJECT_CLASS (context_provider_manager_parent_class)->finalize (obj);
}


GType context_provider_manager_get_type (void) {
	static GType context_provider_manager_type_id = 0;
	if (context_provider_manager_type_id == 0) {
		static const GTypeInfo g_define_type_info = { sizeof (ContextProviderManagerClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) context_provider_manager_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ContextProviderManager), 0, (GInstanceInitFunc) context_provider_manager_instance_init, NULL };
		static const GInterfaceInfo context_provider_dbus_manager_info = { (GInterfaceInitFunc) context_provider_manager_context_provider_dbus_manager_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		context_provider_manager_type_id = g_type_register_static (G_TYPE_OBJECT, "ContextProviderManager", &g_define_type_info, 0);
		g_type_add_interface_static (context_provider_manager_type_id, CONTEXT_PROVIDER_TYPE_DBUS_MANAGER, &context_provider_dbus_manager_info);
	}
	return context_provider_manager_type_id;
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


static void _vala_dbus_register_object (DBusConnection* connection, const char* path, void* object) {
	const _DBusObjectVTable * vtable;
	vtable = g_type_get_qdata (G_TYPE_FROM_INSTANCE (object), g_quark_from_static_string ("DBusObjectVTable"));
	if (vtable) {
		vtable->register_object (connection, path, object);
	} else {
		g_warning ("Object does not implement any D-Bus interface");
	}
}



static void g_cclosure_user_marshal_VOID__STRING_STRING_STRING (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data) {
	typedef void (*GMarshalFunc_VOID__STRING_STRING_STRING) (gpointer data1, const char* arg_1, const char* arg_2, const char* arg_3, gpointer data2);
	register GMarshalFunc_VOID__STRING_STRING_STRING callback;
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
	callback = (GMarshalFunc_VOID__STRING_STRING_STRING) (marshal_data ? marshal_data : cc->callback);
	callback (data1, g_value_get_string (param_values + 1), g_value_get_string (param_values + 2), g_value_get_string (param_values + 3), data2);
}



