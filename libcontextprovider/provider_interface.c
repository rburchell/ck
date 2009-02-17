
#include <provider_interface.h>







void context_provider_provider_get (ContextProviderProvider* self, ContextProviderStringSet* keys, GHashTable* ret, GeeArrayList* unavailable_keys) {
	CONTEXT_PROVIDER_PROVIDER_GET_INTERFACE (self)->get (self, keys, ret, unavailable_keys);
}


void context_provider_provider_keys_subscribed (ContextProviderProvider* self, ContextProviderStringSet* keys_subscribed) {
	CONTEXT_PROVIDER_PROVIDER_GET_INTERFACE (self)->keys_subscribed (self, keys_subscribed);
}


void context_provider_provider_keys_unsubscribed (ContextProviderProvider* self, ContextProviderStringSet* keys_unsubscribed, ContextProviderStringSet* keys_left) {
	CONTEXT_PROVIDER_PROVIDER_GET_INTERFACE (self)->keys_unsubscribed (self, keys_unsubscribed, keys_left);
}


ContextProviderStringSet* context_provider_provider_provided_keys (ContextProviderProvider* self) {
	return CONTEXT_PROVIDER_PROVIDER_GET_INTERFACE (self)->provided_keys (self);
}


static void context_provider_provider_base_init (ContextProviderProviderIface * iface) {
	static gboolean initialized = FALSE;
	if (!initialized) {
		initialized = TRUE;
	}
}


GType context_provider_provider_get_type (void) {
	static GType context_provider_provider_type_id = 0;
	if (context_provider_provider_type_id == 0) {
		static const GTypeInfo g_define_type_info = { sizeof (ContextProviderProviderIface), (GBaseInitFunc) context_provider_provider_base_init, (GBaseFinalizeFunc) NULL, (GClassInitFunc) NULL, (GClassFinalizeFunc) NULL, NULL, 0, 0, (GInstanceInitFunc) NULL, NULL };
		context_provider_provider_type_id = g_type_register_static (G_TYPE_INTERFACE, "ContextProviderProvider", &g_define_type_info, 0);
		g_type_interface_add_prerequisite (context_provider_provider_type_id, G_TYPE_OBJECT);
	}
	return context_provider_provider_type_id;
}




