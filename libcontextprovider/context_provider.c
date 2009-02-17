
#include <context_provider.h>
#include <gee/hashset.h>
#include <gee/collection.h>
#include <manager.h>
#include <cprovider.h>
#include <providers.h>
#include <key_usage_counter.h>
#include <gobject/gvaluecollector.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>

typedef struct _DBusObjectVTable _DBusObjectVTable;

struct _DBusObjectVTable {
	void (*register_object) (DBusConnection*, const char*, void*);
};



gboolean context_provider_initialised = FALSE;
struct _ContextProviderChangeSetPrivate {
	GHashTable* properties;
};

#define CONTEXT_PROVIDER_CHANGE_SET_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), CONTEXT_PROVIDER_TYPE_CHANGE_SET, ContextProviderChangeSetPrivate))
enum  {
	CONTEXT_PROVIDER_CHANGE_SET_DUMMY_PROPERTY
};
static GeeHashSet* context_provider_change_set_changeset_holder = NULL;
static ContextProviderChangeSet* context_provider_change_set_construct (GType object_type);
static ContextProviderChangeSet* context_provider_change_set_new (void);
static GValue* _g_value_dup (GValue* self);
static gpointer context_provider_change_set_parent_class = NULL;
static void context_provider_change_set_finalize (ContextProviderChangeSet* obj);
guint _dynamic_RequestName0 (DBusGProxy* self, const char* param1, guint param2, GError** error);
static void _vala_dbus_register_object (DBusConnection* connection, const char* path, void* object);



/**
 * context_provider_change_set_create:
 *
 * Create a new change set to emit key changes
 * Returns: a new #ContextProviderChangeSet
 */
ContextProviderChangeSet* context_provider_change_set_create (void) {
	ContextProviderChangeSet* s;
	g_assert (context_provider_initialised == TRUE);
	s = context_provider_change_set_new ();
	/*changeset_holder_mutex.lock();*/
	gee_collection_add ((GeeCollection*) context_provider_change_set_changeset_holder, s);
	/*changeset_holder_mutex.unlock();
	 s will have a floating reference*/
	return s;
}


/**
 * context_provider_change_set_commit:
 * @change_set: a #ContextProviderChangeSet to commit
 *
 * Emits the contents of the changeset to all
 * listeners interested in the properties that have changed.
 *
 * Returns: TRUE if changes were emitted
 */
gboolean context_provider_change_set_commit (ContextProviderChangeSet* change_set) {
	ContextProviderManager* manager;
	gboolean ret;
	gboolean _tmp0;
	g_return_val_if_fail (change_set != NULL, FALSE);
	/*take back ownership */
	g_assert (context_provider_initialised == TRUE);
	manager = context_provider_manager_get_instance ();
	ret = context_provider_manager_property_values_changed (manager, change_set->priv->properties, change_set->undeterminable_keys);
	/*changeset_holder_mutex.lock();
	removing it from the changeset_holder causes refcount to go to zero.*/
	gee_collection_remove ((GeeCollection*) context_provider_change_set_changeset_holder, change_set);
	/*changeset_holder_mutex.unlock();*/
	return (_tmp0 = ret, (manager == NULL) ? NULL : (manager = (g_object_unref (manager), NULL)), (change_set == NULL) ? NULL : (change_set = (context_provider_change_set_unref (change_set), NULL)), _tmp0);
}


/**
 * context_provider_change_set_cancel:
 * @change_set : a #ContextProviderChangeSet to cancel
 *
 * Cancels a changeset, cleaning up without emitting the contents.
 */
void context_provider_change_set_cancel (ContextProviderChangeSet* change_set) {
	g_return_if_fail (change_set != NULL);
	/*take back ownership */
	g_assert (context_provider_initialised == TRUE);
	/*changeset_holder_mutex.lock();
	removing it from the changeset_holder causes refcount to go to zero.*/
	gee_collection_remove ((GeeCollection*) context_provider_change_set_changeset_holder, change_set);
	(change_set == NULL) ? NULL : (change_set = (context_provider_change_set_unref (change_set), NULL));
}


/*changeset_holder_mutex.unlock();*/
static ContextProviderChangeSet* context_provider_change_set_construct (GType object_type) {
	ContextProviderChangeSet* self;
	GHashTable* _tmp0;
	GeeArrayList* _tmp1;
	self = (ContextProviderChangeSet*) g_type_create_instance (object_type);
	_tmp0 = NULL;
	self->priv->properties = (_tmp0 = g_hash_table_new (g_str_hash, g_str_equal), (self->priv->properties == NULL) ? NULL : (self->priv->properties = (g_hash_table_unref (self->priv->properties), NULL)), _tmp0);
	_tmp1 = NULL;
	self->undeterminable_keys = (_tmp1 = gee_array_list_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, g_free, g_direct_equal), (self->undeterminable_keys == NULL) ? NULL : (self->undeterminable_keys = (g_object_unref (self->undeterminable_keys), NULL)), _tmp1);
	return self;
}


static ContextProviderChangeSet* context_provider_change_set_new (void) {
	return context_provider_change_set_construct (CONTEXT_PROVIDER_TYPE_CHANGE_SET);
}


ContextProviderChangeSet* context_provider_change_set_construct_from_get (GType object_type, GHashTable* properties, GeeArrayList* undeterminable_keys) {
	ContextProviderChangeSet* self;
	GHashTable* _tmp1;
	GHashTable* _tmp0;
	GeeArrayList* _tmp3;
	GeeArrayList* _tmp2;
	g_return_val_if_fail (properties != NULL, NULL);
	g_return_val_if_fail (undeterminable_keys != NULL, NULL);
	self = (ContextProviderChangeSet*) g_type_create_instance (object_type);
	_tmp1 = NULL;
	_tmp0 = NULL;
	self->priv->properties = (_tmp1 = (_tmp0 = properties, (_tmp0 == NULL) ? NULL : g_hash_table_ref (_tmp0)), (self->priv->properties == NULL) ? NULL : (self->priv->properties = (g_hash_table_unref (self->priv->properties), NULL)), _tmp1);
	_tmp3 = NULL;
	_tmp2 = NULL;
	self->undeterminable_keys = (_tmp3 = (_tmp2 = undeterminable_keys, (_tmp2 == NULL) ? NULL : g_object_ref (_tmp2)), (self->undeterminable_keys == NULL) ? NULL : (self->undeterminable_keys = (g_object_unref (self->undeterminable_keys), NULL)), _tmp3);
	return self;
}


ContextProviderChangeSet* context_provider_change_set_new_from_get (GHashTable* properties, GeeArrayList* undeterminable_keys) {
	return context_provider_change_set_construct_from_get (CONTEXT_PROVIDER_TYPE_CHANGE_SET, properties, undeterminable_keys);
}


static GValue* _g_value_dup (GValue* self) {
	return g_boxed_copy (G_TYPE_VALUE, self);
}


/**
 * context_provider_change_set_add_integer:
 * @self: a #ContextProviderChangeSet to add a value to
 * @key: name of key
 * @value: new value of key
 *
 * Set a key to have an integer value of #value
 */
void context_provider_change_set_add_integer (ContextProviderChangeSet* self, const char* key, gint value) {
	GValue _tmp0 = {0};
	GValue v;
	GValue* _tmp2 = {0};
	const char* _tmp1;
	g_return_if_fail (self != NULL);
	g_return_if_fail (key != NULL);
	g_assert (context_provider_initialised == TRUE);
	v = (g_value_init (&_tmp0, G_TYPE_INT), _tmp0);
	g_value_set_int (&v, value);
	_tmp1 = NULL;
	g_hash_table_insert (self->priv->properties, (_tmp1 = key, (_tmp1 == NULL) ? NULL : g_strdup (_tmp1)), (_tmp2 = &v, (_tmp2 == NULL) ? NULL : _g_value_dup (_tmp2)));
	G_IS_VALUE (&v) ? (g_value_unset (&v), NULL) : NULL;
}


/**
 * context_provider_change_set_add_double:
 * @self: a #ContextProviderChangeSet to add a value to
 * @key: name of key
 * @value: new value of key
 *
 * Set a key to have an floating point value of #value
 */
void context_provider_change_set_add_double (ContextProviderChangeSet* self, const char* key, double value) {
	GValue _tmp0 = {0};
	GValue v;
	GValue* _tmp2 = {0};
	const char* _tmp1;
	g_return_if_fail (self != NULL);
	g_return_if_fail (key != NULL);
	g_assert (context_provider_initialised == TRUE);
	v = (g_value_init (&_tmp0, G_TYPE_DOUBLE), _tmp0);
	g_value_set_double (&v, value);
	_tmp1 = NULL;
	g_hash_table_insert (self->priv->properties, (_tmp1 = key, (_tmp1 == NULL) ? NULL : g_strdup (_tmp1)), (_tmp2 = &v, (_tmp2 == NULL) ? NULL : _g_value_dup (_tmp2)));
	G_IS_VALUE (&v) ? (g_value_unset (&v), NULL) : NULL;
}


/**
 * context_provider_change_set_add_boolean:
 * @self: a #ContextProviderChangeSet to add a value to
 * @key: name of key
 * @value: new value of key
 *
 * Set a key to have an boolean value of #val
 */
void context_provider_change_set_add_boolean (ContextProviderChangeSet* self, const char* key, gboolean value) {
	GValue _tmp0 = {0};
	GValue v;
	GValue* _tmp2 = {0};
	const char* _tmp1;
	g_return_if_fail (self != NULL);
	g_return_if_fail (key != NULL);
	g_assert (context_provider_initialised == TRUE);
	v = (g_value_init (&_tmp0, G_TYPE_BOOLEAN), _tmp0);
	g_value_set_boolean (&v, value);
	_tmp1 = NULL;
	g_hash_table_insert (self->priv->properties, (_tmp1 = key, (_tmp1 == NULL) ? NULL : g_strdup (_tmp1)), (_tmp2 = &v, (_tmp2 == NULL) ? NULL : _g_value_dup (_tmp2)));
	G_IS_VALUE (&v) ? (g_value_unset (&v), NULL) : NULL;
}


/**
 * context_provider_change_set_add_undetermined_key:
 * @self: a #ContextProviderChangeSet to which to add an undeterminable value
 * @key: name of key
 *
 * Marks #key as not having a determinable value.
 */
void context_provider_change_set_add_undetermined_key (ContextProviderChangeSet* self, const char* key) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (key != NULL);
	g_assert (context_provider_initialised == TRUE);
	gee_collection_add ((GeeCollection*) self->undeterminable_keys, key);
}


static void context_provider_value_change_set_init (GValue* value) {
	value->data[0].v_pointer = NULL;
}


static void context_provider_value_change_set_free_value (GValue* value) {
	if (value->data[0].v_pointer) {
		context_provider_change_set_unref (value->data[0].v_pointer);
	}
}


static void context_provider_value_change_set_copy_value (const GValue* src_value, GValue* dest_value) {
	if (src_value->data[0].v_pointer) {
		dest_value->data[0].v_pointer = context_provider_change_set_ref (src_value->data[0].v_pointer);
	} else {
		dest_value->data[0].v_pointer = NULL;
	}
}


static gpointer context_provider_value_change_set_peek_pointer (const GValue* value) {
	return value->data[0].v_pointer;
}


static gchar* context_provider_value_change_set_collect_value (GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	if (collect_values[0].v_pointer) {
		ContextProviderChangeSet* object;
		object = collect_values[0].v_pointer;
		if (object->parent_instance.g_class == NULL) {
			return g_strconcat ("invalid unclassed object pointer for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		} else if (!g_value_type_compatible (G_TYPE_FROM_INSTANCE (object), G_VALUE_TYPE (value))) {
			return g_strconcat ("invalid object type `", g_type_name (G_TYPE_FROM_INSTANCE (object)), "' for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		}
		value->data[0].v_pointer = context_provider_change_set_ref (object);
	} else {
		value->data[0].v_pointer = NULL;
	}
	return NULL;
}


static gchar* context_provider_value_change_set_lcopy_value (const GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	ContextProviderChangeSet** object_p;
	object_p = collect_values[0].v_pointer;
	if (!object_p) {
		return g_strdup_printf ("value location for `%s' passed as NULL", G_VALUE_TYPE_NAME (value));
	}
	if (!value->data[0].v_pointer) {
		*object_p = NULL;
	} else if (collect_flags && G_VALUE_NOCOPY_CONTENTS) {
		*object_p = value->data[0].v_pointer;
	} else {
		*object_p = context_provider_change_set_ref (value->data[0].v_pointer);
	}
	return NULL;
}


GParamSpec* context_provider_param_spec_change_set (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags) {
	ContextProviderParamSpecChangeSet* spec;
	g_return_val_if_fail (g_type_is_a (object_type, CONTEXT_PROVIDER_TYPE_CHANGE_SET), NULL);
	spec = g_param_spec_internal (G_TYPE_PARAM_OBJECT, name, nick, blurb, flags);
	G_PARAM_SPEC (spec)->value_type = object_type;
	return G_PARAM_SPEC (spec);
}


gpointer context_provider_value_get_change_set (const GValue* value) {
	g_return_val_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, CONTEXT_PROVIDER_TYPE_CHANGE_SET), NULL);
	return value->data[0].v_pointer;
}


void context_provider_value_set_change_set (GValue* value, gpointer v_object) {
	ContextProviderChangeSet* old;
	g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, CONTEXT_PROVIDER_TYPE_CHANGE_SET));
	old = value->data[0].v_pointer;
	if (v_object) {
		g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, CONTEXT_PROVIDER_TYPE_CHANGE_SET));
		g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
		value->data[0].v_pointer = v_object;
		context_provider_change_set_ref (value->data[0].v_pointer);
	} else {
		value->data[0].v_pointer = NULL;
	}
	if (old) {
		context_provider_change_set_unref (old);
	}
}


static void context_provider_change_set_class_init (ContextProviderChangeSetClass * klass) {
	context_provider_change_set_parent_class = g_type_class_peek_parent (klass);
	CONTEXT_PROVIDER_CHANGE_SET_CLASS (klass)->finalize = context_provider_change_set_finalize;
	g_type_class_add_private (klass, sizeof (ContextProviderChangeSetPrivate));
	context_provider_change_set_changeset_holder = gee_hash_set_new (CONTEXT_PROVIDER_TYPE_CHANGE_SET, (GBoxedCopyFunc) context_provider_change_set_ref, context_provider_change_set_unref, g_direct_hash, g_direct_equal);
}


static void context_provider_change_set_instance_init (ContextProviderChangeSet * self) {
	self->priv = CONTEXT_PROVIDER_CHANGE_SET_GET_PRIVATE (self);
	self->ref_count = 1;
}


static void context_provider_change_set_finalize (ContextProviderChangeSet* obj) {
	ContextProviderChangeSet * self;
	self = CONTEXT_PROVIDER_CHANGE_SET (obj);
	(self->priv->properties == NULL) ? NULL : (self->priv->properties = (g_hash_table_unref (self->priv->properties), NULL));
	(self->undeterminable_keys == NULL) ? NULL : (self->undeterminable_keys = (g_object_unref (self->undeterminable_keys), NULL));
}


GType context_provider_change_set_get_type (void) {
	static GType context_provider_change_set_type_id = 0;
	if (context_provider_change_set_type_id == 0) {
		static const GTypeValueTable g_define_type_value_table = { context_provider_value_change_set_init, context_provider_value_change_set_free_value, context_provider_value_change_set_copy_value, context_provider_value_change_set_peek_pointer, "p", context_provider_value_change_set_collect_value, "p", context_provider_value_change_set_lcopy_value };
		static const GTypeInfo g_define_type_info = { sizeof (ContextProviderChangeSetClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) context_provider_change_set_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ContextProviderChangeSet), 0, (GInstanceInitFunc) context_provider_change_set_instance_init, &g_define_type_value_table };
		static const GTypeFundamentalInfo g_define_type_fundamental_info = { (G_TYPE_FLAG_CLASSED | G_TYPE_FLAG_INSTANTIATABLE | G_TYPE_FLAG_DERIVABLE | G_TYPE_FLAG_DEEP_DERIVABLE) };
		context_provider_change_set_type_id = g_type_register_fundamental (g_type_fundamental_next (), "ContextProviderChangeSet", &g_define_type_info, &g_define_type_fundamental_info, 0);
	}
	return context_provider_change_set_type_id;
}


gpointer context_provider_change_set_ref (gpointer instance) {
	ContextProviderChangeSet* self;
	self = instance;
	g_atomic_int_inc (&self->ref_count);
	return instance;
}


void context_provider_change_set_unref (gpointer instance) {
	ContextProviderChangeSet* self;
	self = instance;
	if (g_atomic_int_dec_and_test (&self->ref_count)) {
		CONTEXT_PROVIDER_CHANGE_SET_GET_CLASS (self)->finalize (self);
		g_type_free_instance ((GTypeInstance *) self);
	}
}


guint _dynamic_RequestName0 (DBusGProxy* self, const char* param1, guint param2, GError** error) {
	guint result;
	dbus_g_proxy_call (self, "RequestName", error, G_TYPE_STRING, param1, G_TYPE_UINT, param2, G_TYPE_INVALID, G_TYPE_UINT, &result, G_TYPE_INVALID);
	if (*error) {
		return 0U;
	}
	return result;
}


/**
 * context_provider_init:
 * @bus_type: which bus to advertise context on
 * @bus_name: A well-known bus name to register, or NULL if not required.
 *
 * Initialise the Context Provider library.
 *
 * This sets up the D-Bus connection and optionally requests a well known bus name.
 * It must be called before using any other context provider functionality.
 *
 * Returns: TRUE if successful, FALSE otherwise.
 */
gboolean context_provider_init (DBusBusType bus_type, const char* bus_name) {
	GError * inner_error;
	inner_error = NULL;
	{
		ContextProviderManager* manager;
		DBusGConnection* connection;
		if (bus_name != NULL) {
			DBusGConnection* connection;
			DBusGProxy* bus;
			guint request_name_result;
			connection = dbus_g_bus_get (bus_type, &inner_error);
			if (inner_error != NULL) {
				if (inner_error->domain == DBUS_GERROR) {
					goto __catch0_dbus_gerror;
				}
				goto __finally0;
			}
			bus = dbus_g_proxy_new_for_name (connection, "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
			/* try to register service in session bus*/
			request_name_result = _dynamic_RequestName0 (bus, bus_name, (guint) 0, &inner_error);
			if (inner_error != NULL) {
				(connection == NULL) ? NULL : (connection = (dbus_g_connection_unref (connection), NULL));
				(bus == NULL) ? NULL : (bus = (g_object_unref (bus), NULL));
				if (inner_error->domain == DBUS_GERROR) {
					goto __catch0_dbus_gerror;
				}
				goto __finally0;
			}
			if (request_name_result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
				gboolean _tmp0;
				g_debug ("context_provider.vala:221: Unable to register bus name '%s'", bus_name);
				return (_tmp0 = FALSE, (connection == NULL) ? NULL : (connection = (dbus_g_connection_unref (connection), NULL)), (bus == NULL) ? NULL : (bus = (g_object_unref (bus), NULL)), _tmp0);
			}
			(connection == NULL) ? NULL : (connection = (dbus_g_connection_unref (connection), NULL));
			(bus == NULL) ? NULL : (bus = (g_object_unref (bus), NULL));
		}
		g_debug ("context_provider.vala:226: Creating new Manager D-Bus service");
		context_provider_manager_set_bus_type (bus_type);
		manager = context_provider_manager_get_instance ();
		g_debug ("context_provider.vala:233: Registering new Manager D-Bus service");
		connection = dbus_g_bus_get (bus_type, &inner_error);
		if (inner_error != NULL) {
			(manager == NULL) ? NULL : (manager = (g_object_unref (manager), NULL));
			if (inner_error->domain == DBUS_GERROR) {
				goto __catch0_dbus_gerror;
			}
			goto __finally0;
		}
		_vala_dbus_register_object (dbus_g_connection_get_connection (connection), "/org/freedesktop/ContextKit/Manager", (GObject*) manager);
		(manager == NULL) ? NULL : (manager = (g_object_unref (manager), NULL));
		(connection == NULL) ? NULL : (connection = (dbus_g_connection_unref (connection), NULL));
	}
	goto __finally0;
	__catch0_dbus_gerror:
	{
		GError * e;
		e = inner_error;
		inner_error = NULL;
		{
			gboolean _tmp1;
			g_debug ("context_provider.vala:238: Registration failed: %s", e->message);
			return (_tmp1 = FALSE, (e == NULL) ? NULL : (e = (g_error_free (e), NULL)), _tmp1);
		}
	}
	__finally0:
	if (inner_error != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, inner_error->message);
		g_clear_error (&inner_error);
		return FALSE;
	}
	context_provider_initialised = TRUE;
	return TRUE;
}


/**
 * context_provider_install:
 * @provided_keys: a NULL-terminated array of context key names
 * @get_cb: a #ContextProviderGetCallback to be called when values are requested for keys
 * @get_cb_target: user data to pass to #get_cb
 * @subscribed_cb: a #ContextProviderSubscribedCallback to be called when a key is first subscribed to
 * @subscribed_cb_target: user data to pass to #subscribed_cb
 * @unsubscribed_cb: a #ContextProviderUnsubscribedCallback to be called when a key is last unsubscribed from
 * @unsubscribed_cb_target: user data to pass to #unsubscribed_cb
 *
 * Install a new service providing context.
 *
 * Returns: a new #ContextProvider object
 */
ContextProviderProvider* context_provider_install (char** provided_keys, ContextProviderGetCallback get_cb, void* get_cb_target, ContextProviderSubscribedCallback subscribed_cb, void* subscribed_cb_target, ContextProviderUnsubscribedCallback unsubscribed_cb, void* unsubscribed_cb_target) {
	ContextProviderManager* manager;
	ContextProviderStringSet* _tmp0;
	ContextProviderCProvider* _tmp1;
	ContextProviderCProvider* provider;
	ContextProviderProvider* _tmp2;
	g_assert (context_provider_initialised == TRUE);
	manager = context_provider_manager_get_instance ();
	_tmp0 = NULL;
	_tmp1 = NULL;
	provider = (_tmp1 = context_provider_cprovider_new (_tmp0 = context_provider_string_set_new_from_array (provided_keys), get_cb, get_cb_target, subscribed_cb, subscribed_cb_target, unsubscribed_cb, unsubscribed_cb_target), (_tmp0 == NULL) ? NULL : (_tmp0 = (context_provider_string_set_unref (_tmp0), NULL)), _tmp1);
	context_provider_providers_register (context_provider_manager_get_providers (manager), (ContextProviderProvider*) provider);
	_tmp2 = NULL;
	return (_tmp2 = (ContextProviderProvider*) provider, (manager == NULL) ? NULL : (manager = (g_object_unref (manager), NULL)), _tmp2);
}


/**
 * context_provider_remove:
 * @provider: a #ContextProvder created with #context_provider_install.
 *
 * Remove and dealloctate resources for a #ContextProvider created with #context_provider_install
 */
void context_provider_remove (ContextProviderProvider* provider) {
	ContextProviderManager* manager;
	g_return_if_fail (provider != NULL);
	g_assert (context_provider_initialised == TRUE);
	manager = context_provider_manager_get_instance ();
	context_provider_providers_unregister (context_provider_manager_get_providers (manager), provider);
	(manager == NULL) ? NULL : (manager = (g_object_unref (manager), NULL));
}


/**
 * context_provider_no_of_subscribers:
 * @key: the name of a context key
 *
 * Returns: the current number of subscribers for a given context key
 */
gint context_provider_no_of_subscribers (const char* key) {
	ContextProviderManager* manager;
	gint _tmp0;
	g_return_val_if_fail (key != NULL, 0);
	g_assert (context_provider_initialised == TRUE);
	manager = context_provider_manager_get_instance ();
	return (_tmp0 = context_provider_key_usage_counter_number_of_subscribers (context_provider_manager_get_key_counter (manager), key), (manager == NULL) ? NULL : (manager = (g_object_unref (manager), NULL)), _tmp0);
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




