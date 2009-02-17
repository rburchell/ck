
#include <key_usage_counter.h>
#include <gee/hashmap.h>
#include <gee/map.h>
#include <manager.h>
#include <providers.h>
#include <gobject/gvaluecollector.h>




struct _ContextProviderKeyUsageCounterPrivate {
	GeeHashMap* no_of_subscribers;
	ContextProviderStringSet* _subscribed_keys;
};

#define CONTEXT_PROVIDER_KEY_USAGE_COUNTER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), CONTEXT_PROVIDER_TYPE_KEY_USAGE_COUNTER, ContextProviderKeyUsageCounterPrivate))
enum  {
	CONTEXT_PROVIDER_KEY_USAGE_COUNTER_DUMMY_PROPERTY
};
static void context_provider_key_usage_counter_set_subscribed_keys (ContextProviderKeyUsageCounter* self, ContextProviderStringSet* value);
static gpointer context_provider_key_usage_counter_parent_class = NULL;
static void context_provider_key_usage_counter_finalize (ContextProviderKeyUsageCounter* obj);



ContextProviderKeyUsageCounter* context_provider_key_usage_counter_construct (GType object_type) {
	ContextProviderKeyUsageCounter* self;
	ContextProviderStringSet* _tmp0;
	self = (ContextProviderKeyUsageCounter*) g_type_create_instance (object_type);
	_tmp0 = NULL;
	context_provider_key_usage_counter_set_subscribed_keys (self, _tmp0 = context_provider_string_set_new ());
	(_tmp0 == NULL) ? NULL : (_tmp0 = (context_provider_string_set_unref (_tmp0), NULL));
	return self;
}


ContextProviderKeyUsageCounter* context_provider_key_usage_counter_new (void) {
	return context_provider_key_usage_counter_construct (CONTEXT_PROVIDER_TYPE_KEY_USAGE_COUNTER);
}


void context_provider_key_usage_counter_add (ContextProviderKeyUsageCounter* self, ContextProviderStringSet* keys) {
	ContextProviderStringSet* first_subscribed_keys;
	ContextProviderStringSet* _tmp1;
	g_return_if_fail (self != NULL);
	g_return_if_fail (keys != NULL);
	first_subscribed_keys = context_provider_string_set_new ();
	{
		ContextProviderStringSetIter* _key_it;
		_key_it = context_provider_string_set_iterator (keys);
		while (context_provider_string_set_iter_next (_key_it)) {
			char* key;
			key = context_provider_string_set_iter_get (_key_it);
			if (gee_map_contains ((GeeMap*) self->priv->no_of_subscribers, key)) {
				gint old_value;
				old_value = GPOINTER_TO_INT (gee_map_get ((GeeMap*) self->priv->no_of_subscribers, key));
				gee_map_set ((GeeMap*) self->priv->no_of_subscribers, key, GINT_TO_POINTER (old_value + 1));
				if (old_value == 0) {
					/* This is the first subscribed to the key*/
					context_provider_string_set_add (first_subscribed_keys, key);
				}
				g_debug ("key_usage_counter.vala:27: Subscriber count of %s is now %d", key, old_value + 1);
			} else {
				/* Key name not present in the key table*/
				gee_map_set ((GeeMap*) self->priv->no_of_subscribers, key, GINT_TO_POINTER (1));
				context_provider_string_set_add (first_subscribed_keys, key);
				g_debug ("key_usage_counter.vala:33: Subscriber count of %s is now %d", key, 1);
			}
			key = (g_free (key), NULL);
		}
		(_key_it == NULL) ? NULL : (_key_it = (context_provider_string_set_iter_unref (_key_it), NULL));
	}
	if (context_provider_string_set_size (first_subscribed_keys) > 0) {
		ContextProviderManager* _tmp0;
		/* Signal that some new keys were subscribed to*/
		_tmp0 = NULL;
		context_provider_providers_first_subscribed (context_provider_manager_get_providers (_tmp0 = context_provider_manager_get_instance ()), first_subscribed_keys);
		(_tmp0 == NULL) ? NULL : (_tmp0 = (g_object_unref (_tmp0), NULL));
	}
	_tmp1 = NULL;
	context_provider_key_usage_counter_set_subscribed_keys (self, _tmp1 = context_provider_string_set_new_union (self->priv->_subscribed_keys, first_subscribed_keys));
	(_tmp1 == NULL) ? NULL : (_tmp1 = (context_provider_string_set_unref (_tmp1), NULL));
	(first_subscribed_keys == NULL) ? NULL : (first_subscribed_keys = (context_provider_string_set_unref (first_subscribed_keys), NULL));
}


void context_provider_key_usage_counter_remove (ContextProviderKeyUsageCounter* self, ContextProviderStringSet* keys) {
	ContextProviderStringSet* last_unsubscribed_keys;
	ContextProviderStringSet* _tmp0;
	g_return_if_fail (self != NULL);
	g_return_if_fail (keys != NULL);
	last_unsubscribed_keys = context_provider_string_set_new ();
	{
		ContextProviderStringSetIter* _key_it;
		_key_it = context_provider_string_set_iterator (keys);
		while (context_provider_string_set_iter_next (_key_it)) {
			char* key;
			key = context_provider_string_set_iter_get (_key_it);
			if (gee_map_contains ((GeeMap*) self->priv->no_of_subscribers, key)) {
				gint old_value;
				old_value = GPOINTER_TO_INT (gee_map_get ((GeeMap*) self->priv->no_of_subscribers, key));
				if (old_value >= 1) {
					/* Do not store negative values*/
					gee_map_set ((GeeMap*) self->priv->no_of_subscribers, key, GINT_TO_POINTER (old_value - 1));
				}
				if (old_value <= 1) {
					/* The last subscriber for this key unsubscribed*/
					context_provider_string_set_add (last_unsubscribed_keys, key);
				}
				g_debug ("key_usage_counter.vala:61: Subscriber count of %s is now %d", key, old_value - 1);
			} else {
				/* This should not happen*/
				g_debug ("key_usage_counter.vala:65: Error: decreasing the subscriber count of an unknown key.");
			}
			key = (g_free (key), NULL);
		}
		(_key_it == NULL) ? NULL : (_key_it = (context_provider_string_set_iter_unref (_key_it), NULL));
	}
	_tmp0 = NULL;
	context_provider_key_usage_counter_set_subscribed_keys (self, _tmp0 = context_provider_string_set_new_difference (self->priv->_subscribed_keys, last_unsubscribed_keys));
	(_tmp0 == NULL) ? NULL : (_tmp0 = (context_provider_string_set_unref (_tmp0), NULL));
	if (context_provider_string_set_size (last_unsubscribed_keys) > 0) {
		ContextProviderManager* _tmp1;
		/* Signal that some keys are not subscribed to anymore*/
		_tmp1 = NULL;
		context_provider_providers_last_unsubscribed (context_provider_manager_get_providers (_tmp1 = context_provider_manager_get_instance ()), last_unsubscribed_keys, self->priv->_subscribed_keys);
		(_tmp1 == NULL) ? NULL : (_tmp1 = (g_object_unref (_tmp1), NULL));
	}
	(last_unsubscribed_keys == NULL) ? NULL : (last_unsubscribed_keys = (context_provider_string_set_unref (last_unsubscribed_keys), NULL));
}


gint context_provider_key_usage_counter_number_of_subscribers (ContextProviderKeyUsageCounter* self, const char* key) {
	g_return_val_if_fail (self != NULL, 0);
	g_return_val_if_fail (key != NULL, 0);
	if (gee_map_contains ((GeeMap*) self->priv->no_of_subscribers, key)) {
		return GPOINTER_TO_INT (gee_map_get ((GeeMap*) self->priv->no_of_subscribers, key));
	}
	return 0;
}


ContextProviderStringSet* context_provider_key_usage_counter_get_subscribed_keys (ContextProviderKeyUsageCounter* self) {
	g_return_val_if_fail (self != NULL, NULL);
	return self->priv->_subscribed_keys;
}


static void context_provider_key_usage_counter_set_subscribed_keys (ContextProviderKeyUsageCounter* self, ContextProviderStringSet* value) {
	ContextProviderStringSet* _tmp2;
	ContextProviderStringSet* _tmp1;
	g_return_if_fail (self != NULL);
	_tmp2 = NULL;
	_tmp1 = NULL;
	self->priv->_subscribed_keys = (_tmp2 = (_tmp1 = value, (_tmp1 == NULL) ? NULL : context_provider_string_set_ref (_tmp1)), (self->priv->_subscribed_keys == NULL) ? NULL : (self->priv->_subscribed_keys = (context_provider_string_set_unref (self->priv->_subscribed_keys), NULL)), _tmp2);
}


static void context_provider_value_key_usage_counter_init (GValue* value) {
	value->data[0].v_pointer = NULL;
}


static void context_provider_value_key_usage_counter_free_value (GValue* value) {
	if (value->data[0].v_pointer) {
		context_provider_key_usage_counter_unref (value->data[0].v_pointer);
	}
}


static void context_provider_value_key_usage_counter_copy_value (const GValue* src_value, GValue* dest_value) {
	if (src_value->data[0].v_pointer) {
		dest_value->data[0].v_pointer = context_provider_key_usage_counter_ref (src_value->data[0].v_pointer);
	} else {
		dest_value->data[0].v_pointer = NULL;
	}
}


static gpointer context_provider_value_key_usage_counter_peek_pointer (const GValue* value) {
	return value->data[0].v_pointer;
}


static gchar* context_provider_value_key_usage_counter_collect_value (GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	if (collect_values[0].v_pointer) {
		ContextProviderKeyUsageCounter* object;
		object = collect_values[0].v_pointer;
		if (object->parent_instance.g_class == NULL) {
			return g_strconcat ("invalid unclassed object pointer for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		} else if (!g_value_type_compatible (G_TYPE_FROM_INSTANCE (object), G_VALUE_TYPE (value))) {
			return g_strconcat ("invalid object type `", g_type_name (G_TYPE_FROM_INSTANCE (object)), "' for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		}
		value->data[0].v_pointer = context_provider_key_usage_counter_ref (object);
	} else {
		value->data[0].v_pointer = NULL;
	}
	return NULL;
}


static gchar* context_provider_value_key_usage_counter_lcopy_value (const GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	ContextProviderKeyUsageCounter** object_p;
	object_p = collect_values[0].v_pointer;
	if (!object_p) {
		return g_strdup_printf ("value location for `%s' passed as NULL", G_VALUE_TYPE_NAME (value));
	}
	if (!value->data[0].v_pointer) {
		*object_p = NULL;
	} else if (collect_flags && G_VALUE_NOCOPY_CONTENTS) {
		*object_p = value->data[0].v_pointer;
	} else {
		*object_p = context_provider_key_usage_counter_ref (value->data[0].v_pointer);
	}
	return NULL;
}


GParamSpec* context_provider_param_spec_key_usage_counter (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags) {
	ContextProviderParamSpecKeyUsageCounter* spec;
	g_return_val_if_fail (g_type_is_a (object_type, CONTEXT_PROVIDER_TYPE_KEY_USAGE_COUNTER), NULL);
	spec = g_param_spec_internal (G_TYPE_PARAM_OBJECT, name, nick, blurb, flags);
	G_PARAM_SPEC (spec)->value_type = object_type;
	return G_PARAM_SPEC (spec);
}


gpointer context_provider_value_get_key_usage_counter (const GValue* value) {
	g_return_val_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, CONTEXT_PROVIDER_TYPE_KEY_USAGE_COUNTER), NULL);
	return value->data[0].v_pointer;
}


void context_provider_value_set_key_usage_counter (GValue* value, gpointer v_object) {
	ContextProviderKeyUsageCounter* old;
	g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, CONTEXT_PROVIDER_TYPE_KEY_USAGE_COUNTER));
	old = value->data[0].v_pointer;
	if (v_object) {
		g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, CONTEXT_PROVIDER_TYPE_KEY_USAGE_COUNTER));
		g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
		value->data[0].v_pointer = v_object;
		context_provider_key_usage_counter_ref (value->data[0].v_pointer);
	} else {
		value->data[0].v_pointer = NULL;
	}
	if (old) {
		context_provider_key_usage_counter_unref (old);
	}
}


static void context_provider_key_usage_counter_class_init (ContextProviderKeyUsageCounterClass * klass) {
	context_provider_key_usage_counter_parent_class = g_type_class_peek_parent (klass);
	CONTEXT_PROVIDER_KEY_USAGE_COUNTER_CLASS (klass)->finalize = context_provider_key_usage_counter_finalize;
	g_type_class_add_private (klass, sizeof (ContextProviderKeyUsageCounterPrivate));
}


static void context_provider_key_usage_counter_instance_init (ContextProviderKeyUsageCounter * self) {
	self->priv = CONTEXT_PROVIDER_KEY_USAGE_COUNTER_GET_PRIVATE (self);
	self->priv->no_of_subscribers = gee_hash_map_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, g_free, G_TYPE_INT, NULL, NULL, g_str_hash, g_str_equal, g_direct_equal);
	self->ref_count = 1;
}


static void context_provider_key_usage_counter_finalize (ContextProviderKeyUsageCounter* obj) {
	ContextProviderKeyUsageCounter * self;
	self = CONTEXT_PROVIDER_KEY_USAGE_COUNTER (obj);
	(self->priv->no_of_subscribers == NULL) ? NULL : (self->priv->no_of_subscribers = (g_object_unref (self->priv->no_of_subscribers), NULL));
	(self->priv->_subscribed_keys == NULL) ? NULL : (self->priv->_subscribed_keys = (context_provider_string_set_unref (self->priv->_subscribed_keys), NULL));
}


GType context_provider_key_usage_counter_get_type (void) {
	static GType context_provider_key_usage_counter_type_id = 0;
	if (context_provider_key_usage_counter_type_id == 0) {
		static const GTypeValueTable g_define_type_value_table = { context_provider_value_key_usage_counter_init, context_provider_value_key_usage_counter_free_value, context_provider_value_key_usage_counter_copy_value, context_provider_value_key_usage_counter_peek_pointer, "p", context_provider_value_key_usage_counter_collect_value, "p", context_provider_value_key_usage_counter_lcopy_value };
		static const GTypeInfo g_define_type_info = { sizeof (ContextProviderKeyUsageCounterClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) context_provider_key_usage_counter_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ContextProviderKeyUsageCounter), 0, (GInstanceInitFunc) context_provider_key_usage_counter_instance_init, &g_define_type_value_table };
		static const GTypeFundamentalInfo g_define_type_fundamental_info = { (G_TYPE_FLAG_CLASSED | G_TYPE_FLAG_INSTANTIATABLE | G_TYPE_FLAG_DERIVABLE | G_TYPE_FLAG_DEEP_DERIVABLE) };
		context_provider_key_usage_counter_type_id = g_type_register_fundamental (g_type_fundamental_next (), "ContextProviderKeyUsageCounter", &g_define_type_info, &g_define_type_fundamental_info, 0);
	}
	return context_provider_key_usage_counter_type_id;
}


gpointer context_provider_key_usage_counter_ref (gpointer instance) {
	ContextProviderKeyUsageCounter* self;
	self = instance;
	g_atomic_int_inc (&self->ref_count);
	return instance;
}


void context_provider_key_usage_counter_unref (gpointer instance) {
	ContextProviderKeyUsageCounter* self;
	self = instance;
	if (g_atomic_int_dec_and_test (&self->ref_count)) {
		CONTEXT_PROVIDER_KEY_USAGE_COUNTER_GET_CLASS (self)->finalize (self);
		g_type_free_instance ((GTypeInstance *) self);
	}
}




