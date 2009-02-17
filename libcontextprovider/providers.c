
#include <providers.h>
#include <stdlib.h>
#include <string.h>
#include <gee/collection.h>
#include <gee/iterable.h>
#include <gee/iterator.h>
#include <gobject/gvaluecollector.h>




struct _ContextProviderProvidersPrivate {
	GeeArrayList* providers;
	ContextProviderStringSet* _valid_keys;
};

#define CONTEXT_PROVIDER_PROVIDERS_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), CONTEXT_PROVIDER_TYPE_PROVIDERS, ContextProviderProvidersPrivate))
enum  {
	CONTEXT_PROVIDER_PROVIDERS_DUMMY_PROPERTY
};
static void context_provider_providers_set_valid_keys (ContextProviderProviders* self, ContextProviderStringSet* value);
static gpointer context_provider_providers_parent_class = NULL;
static void context_provider_providers_finalize (ContextProviderProviders* obj);



ContextProviderProviders* context_provider_providers_construct (GType object_type) {
	ContextProviderProviders* self;
	ContextProviderStringSet* _tmp0;
	self = (ContextProviderProviders*) g_type_create_instance (object_type);
	_tmp0 = NULL;
	context_provider_providers_set_valid_keys (self, _tmp0 = context_provider_string_set_new ());
	(_tmp0 == NULL) ? NULL : (_tmp0 = (context_provider_string_set_unref (_tmp0), NULL));
	return self;
}


ContextProviderProviders* context_provider_providers_new (void) {
	return context_provider_providers_construct (CONTEXT_PROVIDER_TYPE_PROVIDERS);
}


void context_provider_providers_register (ContextProviderProviders* self, ContextProviderProvider* provider) {
	char* _tmp1;
	ContextProviderStringSet* _tmp0;
	ContextProviderStringSet* _tmp3;
	ContextProviderStringSet* _tmp2;
	g_return_if_fail (self != NULL);
	g_return_if_fail (provider != NULL);
	_tmp1 = NULL;
	_tmp0 = NULL;
	g_debug ("providers.vala:17: New provider registered: %s (%p)", _tmp1 = context_provider_string_set_debug (_tmp0 = context_provider_provider_provided_keys (provider)), provider);
	_tmp1 = (g_free (_tmp1), NULL);
	(_tmp0 == NULL) ? NULL : (_tmp0 = (context_provider_string_set_unref (_tmp0), NULL));
	gee_collection_add ((GeeCollection*) self->priv->providers, provider);
	_tmp3 = NULL;
	_tmp2 = NULL;
	context_provider_providers_set_valid_keys (self, _tmp3 = context_provider_string_set_new_union (self->priv->_valid_keys, _tmp2 = context_provider_provider_provided_keys (provider)));
	(_tmp3 == NULL) ? NULL : (_tmp3 = (context_provider_string_set_unref (_tmp3), NULL));
	(_tmp2 == NULL) ? NULL : (_tmp2 = (context_provider_string_set_unref (_tmp2), NULL));
}


void context_provider_providers_unregister (ContextProviderProviders* self, ContextProviderProvider* provider) {
	char* _tmp1;
	ContextProviderStringSet* _tmp0;
	ContextProviderStringSet* s;
	g_return_if_fail (self != NULL);
	g_return_if_fail (provider != NULL);
	_tmp1 = NULL;
	_tmp0 = NULL;
	g_debug ("providers.vala:23: Provider unregistered: %s (%p)", _tmp1 = context_provider_string_set_debug (_tmp0 = context_provider_provider_provided_keys (provider)), provider);
	_tmp1 = (g_free (_tmp1), NULL);
	(_tmp0 == NULL) ? NULL : (_tmp0 = (context_provider_string_set_unref (_tmp0), NULL));
	gee_collection_remove ((GeeCollection*) self->priv->providers, provider);
	s = context_provider_string_set_new ();
	/* a bit inefficient, but it'll do for now...
	   TODO: make the stringset stuff not use ctors..
	 */
	{
		GeeIterator* _p_it;
		/* a bit inefficient, but it'll do for now...
		   TODO: make the stringset stuff not use ctors..
		 */
		_p_it = gee_iterable_iterator ((GeeIterable*) self->priv->providers);
		/* a bit inefficient, but it'll do for now...
		   TODO: make the stringset stuff not use ctors..
		 */
		while (gee_iterator_next (_p_it)) {
			ContextProviderProvider* p;
			ContextProviderStringSet* _tmp3;
			ContextProviderStringSet* _tmp2;
			/* a bit inefficient, but it'll do for now...
			   TODO: make the stringset stuff not use ctors..
			 */
			p = (ContextProviderProvider*) gee_iterator_get (_p_it);
			_tmp3 = NULL;
			_tmp2 = NULL;
			s = (_tmp3 = context_provider_string_set_new_union (s, _tmp2 = context_provider_provider_provided_keys (p)), (s == NULL) ? NULL : (s = (context_provider_string_set_unref (s), NULL)), _tmp3);
			(_tmp2 == NULL) ? NULL : (_tmp2 = (context_provider_string_set_unref (_tmp2), NULL));
			(p == NULL) ? NULL : (p = (g_object_unref (p), NULL));
		}
		(_p_it == NULL) ? NULL : (_p_it = (g_object_unref (_p_it), NULL));
	}
	context_provider_providers_set_valid_keys (self, s);
	(s == NULL) ? NULL : (s = (context_provider_string_set_unref (s), NULL));
}


GeeArrayList* context_provider_providers_get (ContextProviderProviders* self, ContextProviderStringSet* keys, GHashTable* values) {
	char* _tmp0;
	GeeArrayList* unavail;
	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (keys != NULL, NULL);
	g_return_val_if_fail (values != NULL, NULL);
	_tmp0 = NULL;
	g_debug ("providers.vala:37: Providers.get called (%s)", _tmp0 = context_provider_string_set_debug (keys));
	_tmp0 = (g_free (_tmp0), NULL);
	unavail = gee_array_list_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, g_free, g_direct_equal);
	{
		GeeIterator* _provider_it;
		_provider_it = gee_iterable_iterator ((GeeIterable*) self->priv->providers);
		while (gee_iterator_next (_provider_it)) {
			ContextProviderProvider* provider;
			ContextProviderStringSet* _tmp1;
			ContextProviderStringSet* _tmp2;
			ContextProviderStringSet* intersection;
			provider = (ContextProviderProvider*) gee_iterator_get (_provider_it);
			_tmp1 = NULL;
			_tmp2 = NULL;
			intersection = (_tmp2 = context_provider_string_set_new_intersection (keys, _tmp1 = context_provider_provider_provided_keys (provider)), (_tmp1 == NULL) ? NULL : (_tmp1 = (context_provider_string_set_unref (_tmp1), NULL)), _tmp2);
			if (context_provider_string_set_size (intersection) > 0) {
				g_debug ("providers.vala:42: calling provider %p", provider);
				context_provider_provider_get (provider, keys, values, unavail);
			}
			(provider == NULL) ? NULL : (provider = (g_object_unref (provider), NULL));
			(intersection == NULL) ? NULL : (intersection = (context_provider_string_set_unref (intersection), NULL));
		}
		(_provider_it == NULL) ? NULL : (_provider_it = (g_object_unref (_provider_it), NULL));
	}
	return unavail;
}


void context_provider_providers_first_subscribed (ContextProviderProviders* self, ContextProviderStringSet* keys) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (keys != NULL);
	{
		GeeIterator* _provider_it;
		_provider_it = gee_iterable_iterator ((GeeIterable*) self->priv->providers);
		while (gee_iterator_next (_provider_it)) {
			ContextProviderProvider* provider;
			ContextProviderStringSet* _tmp0;
			ContextProviderStringSet* _tmp1;
			ContextProviderStringSet* intersection;
			provider = (ContextProviderProvider*) gee_iterator_get (_provider_it);
			_tmp0 = NULL;
			_tmp1 = NULL;
			intersection = (_tmp1 = context_provider_string_set_new_intersection (keys, _tmp0 = context_provider_provider_provided_keys (provider)), (_tmp0 == NULL) ? NULL : (_tmp0 = (context_provider_string_set_unref (_tmp0), NULL)), _tmp1);
			if (context_provider_string_set_size (intersection) > 0) {
				context_provider_provider_keys_subscribed (provider, keys);
			}
			(provider == NULL) ? NULL : (provider = (g_object_unref (provider), NULL));
			(intersection == NULL) ? NULL : (intersection = (context_provider_string_set_unref (intersection), NULL));
		}
		(_provider_it == NULL) ? NULL : (_provider_it = (g_object_unref (_provider_it), NULL));
	}
}


void context_provider_providers_last_unsubscribed (ContextProviderProviders* self, ContextProviderStringSet* keys_unsubscribed, ContextProviderStringSet* keys_remaining) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (keys_unsubscribed != NULL);
	g_return_if_fail (keys_remaining != NULL);
	{
		GeeIterator* _provider_it;
		_provider_it = gee_iterable_iterator ((GeeIterable*) self->priv->providers);
		while (gee_iterator_next (_provider_it)) {
			ContextProviderProvider* provider;
			ContextProviderStringSet* _tmp0;
			ContextProviderStringSet* _tmp1;
			ContextProviderStringSet* intersection;
			provider = (ContextProviderProvider*) gee_iterator_get (_provider_it);
			_tmp0 = NULL;
			_tmp1 = NULL;
			intersection = (_tmp1 = context_provider_string_set_new_intersection (keys_unsubscribed, _tmp0 = context_provider_provider_provided_keys (provider)), (_tmp0 == NULL) ? NULL : (_tmp0 = (context_provider_string_set_unref (_tmp0), NULL)), _tmp1);
			if (context_provider_string_set_size (intersection) > 0) {
				ContextProviderStringSet* _tmp2;
				ContextProviderStringSet* _tmp3;
				ContextProviderStringSet* intersection_remaining;
				_tmp2 = NULL;
				_tmp3 = NULL;
				intersection_remaining = (_tmp3 = context_provider_string_set_new_intersection (keys_remaining, _tmp2 = context_provider_provider_provided_keys (provider)), (_tmp2 == NULL) ? NULL : (_tmp2 = (context_provider_string_set_unref (_tmp2), NULL)), _tmp3);
				context_provider_provider_keys_unsubscribed (provider, intersection, intersection_remaining);
				(intersection_remaining == NULL) ? NULL : (intersection_remaining = (context_provider_string_set_unref (intersection_remaining), NULL));
			}
			(provider == NULL) ? NULL : (provider = (g_object_unref (provider), NULL));
			(intersection == NULL) ? NULL : (intersection = (context_provider_string_set_unref (intersection), NULL));
		}
		(_provider_it == NULL) ? NULL : (_provider_it = (g_object_unref (_provider_it), NULL));
	}
}


ContextProviderStringSet* context_provider_providers_get_valid_keys (ContextProviderProviders* self) {
	g_return_val_if_fail (self != NULL, NULL);
	return self->priv->_valid_keys;
}


static void context_provider_providers_set_valid_keys (ContextProviderProviders* self, ContextProviderStringSet* value) {
	ContextProviderStringSet* _tmp2;
	ContextProviderStringSet* _tmp1;
	g_return_if_fail (self != NULL);
	_tmp2 = NULL;
	_tmp1 = NULL;
	self->priv->_valid_keys = (_tmp2 = (_tmp1 = value, (_tmp1 == NULL) ? NULL : context_provider_string_set_ref (_tmp1)), (self->priv->_valid_keys == NULL) ? NULL : (self->priv->_valid_keys = (context_provider_string_set_unref (self->priv->_valid_keys), NULL)), _tmp2);
}


static void context_provider_value_providers_init (GValue* value) {
	value->data[0].v_pointer = NULL;
}


static void context_provider_value_providers_free_value (GValue* value) {
	if (value->data[0].v_pointer) {
		context_provider_providers_unref (value->data[0].v_pointer);
	}
}


static void context_provider_value_providers_copy_value (const GValue* src_value, GValue* dest_value) {
	if (src_value->data[0].v_pointer) {
		dest_value->data[0].v_pointer = context_provider_providers_ref (src_value->data[0].v_pointer);
	} else {
		dest_value->data[0].v_pointer = NULL;
	}
}


static gpointer context_provider_value_providers_peek_pointer (const GValue* value) {
	return value->data[0].v_pointer;
}


static gchar* context_provider_value_providers_collect_value (GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	if (collect_values[0].v_pointer) {
		ContextProviderProviders* object;
		object = collect_values[0].v_pointer;
		if (object->parent_instance.g_class == NULL) {
			return g_strconcat ("invalid unclassed object pointer for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		} else if (!g_value_type_compatible (G_TYPE_FROM_INSTANCE (object), G_VALUE_TYPE (value))) {
			return g_strconcat ("invalid object type `", g_type_name (G_TYPE_FROM_INSTANCE (object)), "' for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		}
		value->data[0].v_pointer = context_provider_providers_ref (object);
	} else {
		value->data[0].v_pointer = NULL;
	}
	return NULL;
}


static gchar* context_provider_value_providers_lcopy_value (const GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	ContextProviderProviders** object_p;
	object_p = collect_values[0].v_pointer;
	if (!object_p) {
		return g_strdup_printf ("value location for `%s' passed as NULL", G_VALUE_TYPE_NAME (value));
	}
	if (!value->data[0].v_pointer) {
		*object_p = NULL;
	} else if (collect_flags && G_VALUE_NOCOPY_CONTENTS) {
		*object_p = value->data[0].v_pointer;
	} else {
		*object_p = context_provider_providers_ref (value->data[0].v_pointer);
	}
	return NULL;
}


GParamSpec* context_provider_param_spec_providers (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags) {
	ContextProviderParamSpecProviders* spec;
	g_return_val_if_fail (g_type_is_a (object_type, CONTEXT_PROVIDER_TYPE_PROVIDERS), NULL);
	spec = g_param_spec_internal (G_TYPE_PARAM_OBJECT, name, nick, blurb, flags);
	G_PARAM_SPEC (spec)->value_type = object_type;
	return G_PARAM_SPEC (spec);
}


gpointer context_provider_value_get_providers (const GValue* value) {
	g_return_val_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, CONTEXT_PROVIDER_TYPE_PROVIDERS), NULL);
	return value->data[0].v_pointer;
}


void context_provider_value_set_providers (GValue* value, gpointer v_object) {
	ContextProviderProviders* old;
	g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, CONTEXT_PROVIDER_TYPE_PROVIDERS));
	old = value->data[0].v_pointer;
	if (v_object) {
		g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, CONTEXT_PROVIDER_TYPE_PROVIDERS));
		g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
		value->data[0].v_pointer = v_object;
		context_provider_providers_ref (value->data[0].v_pointer);
	} else {
		value->data[0].v_pointer = NULL;
	}
	if (old) {
		context_provider_providers_unref (old);
	}
}


static void context_provider_providers_class_init (ContextProviderProvidersClass * klass) {
	context_provider_providers_parent_class = g_type_class_peek_parent (klass);
	CONTEXT_PROVIDER_PROVIDERS_CLASS (klass)->finalize = context_provider_providers_finalize;
	g_type_class_add_private (klass, sizeof (ContextProviderProvidersPrivate));
}


static void context_provider_providers_instance_init (ContextProviderProviders * self) {
	self->priv = CONTEXT_PROVIDER_PROVIDERS_GET_PRIVATE (self);
	self->priv->providers = gee_array_list_new (CONTEXT_PROVIDER_TYPE_PROVIDER, (GBoxedCopyFunc) g_object_ref, g_object_unref, g_direct_equal);
	self->ref_count = 1;
}


static void context_provider_providers_finalize (ContextProviderProviders* obj) {
	ContextProviderProviders * self;
	self = CONTEXT_PROVIDER_PROVIDERS (obj);
	(self->priv->providers == NULL) ? NULL : (self->priv->providers = (g_object_unref (self->priv->providers), NULL));
	(self->priv->_valid_keys == NULL) ? NULL : (self->priv->_valid_keys = (context_provider_string_set_unref (self->priv->_valid_keys), NULL));
}


GType context_provider_providers_get_type (void) {
	static GType context_provider_providers_type_id = 0;
	if (context_provider_providers_type_id == 0) {
		static const GTypeValueTable g_define_type_value_table = { context_provider_value_providers_init, context_provider_value_providers_free_value, context_provider_value_providers_copy_value, context_provider_value_providers_peek_pointer, "p", context_provider_value_providers_collect_value, "p", context_provider_value_providers_lcopy_value };
		static const GTypeInfo g_define_type_info = { sizeof (ContextProviderProvidersClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) context_provider_providers_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ContextProviderProviders), 0, (GInstanceInitFunc) context_provider_providers_instance_init, &g_define_type_value_table };
		static const GTypeFundamentalInfo g_define_type_fundamental_info = { (G_TYPE_FLAG_CLASSED | G_TYPE_FLAG_INSTANTIATABLE | G_TYPE_FLAG_DERIVABLE | G_TYPE_FLAG_DEEP_DERIVABLE) };
		context_provider_providers_type_id = g_type_register_fundamental (g_type_fundamental_next (), "ContextProviderProviders", &g_define_type_info, &g_define_type_fundamental_info, 0);
	}
	return context_provider_providers_type_id;
}


gpointer context_provider_providers_ref (gpointer instance) {
	ContextProviderProviders* self;
	self = instance;
	g_atomic_int_inc (&self->ref_count);
	return instance;
}


void context_provider_providers_unref (gpointer instance) {
	ContextProviderProviders* self;
	self = instance;
	if (g_atomic_int_dec_and_test (&self->ref_count)) {
		CONTEXT_PROVIDER_PROVIDERS_GET_CLASS (self)->finalize (self);
		g_type_free_instance ((GTypeInstance *) self);
	}
}




