
#include <cprovider.h>
#include <stdlib.h>
#include <string.h>




struct _ContextProviderCProviderPrivate {
	ContextProviderGetCallback get_cb;
	gpointer get_cb_target;
	ContextProviderSubscribedCallback first_cb;
	gpointer first_cb_target;
	ContextProviderUnsubscribedCallback last_cb;
	gpointer last_cb_target;
	ContextProviderStringSet* keys;
};

#define CONTEXT_PROVIDER_CPROVIDER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), CONTEXT_PROVIDER_TYPE_CPROVIDER, ContextProviderCProviderPrivate))
enum  {
	CONTEXT_PROVIDER_CPROVIDER_DUMMY_PROPERTY
};
static void context_provider_cprovider_real_get (ContextProviderProvider* base, ContextProviderStringSet* keys, GHashTable* ret, GeeArrayList* unavail);
static void context_provider_cprovider_real_keys_subscribed (ContextProviderProvider* base, ContextProviderStringSet* keys);
static void context_provider_cprovider_real_keys_unsubscribed (ContextProviderProvider* base, ContextProviderStringSet* keys_unsubscribed, ContextProviderStringSet* keys_remaining);
static ContextProviderStringSet* context_provider_cprovider_real_provided_keys (ContextProviderProvider* base);
static gpointer context_provider_cprovider_parent_class = NULL;
static ContextProviderProviderIface* context_provider_cprovider_context_provider_provider_parent_iface = NULL;
static void context_provider_cprovider_finalize (GObject* obj);



ContextProviderCProvider* context_provider_cprovider_construct (GType object_type, ContextProviderStringSet* keys, ContextProviderGetCallback get_cb, void* get_cb_target, ContextProviderSubscribedCallback first_cb, void* first_cb_target, ContextProviderUnsubscribedCallback last_cb, void* last_cb_target) {
	ContextProviderCProvider * self;
	ContextProviderGetCallback _tmp0;
	ContextProviderSubscribedCallback _tmp1;
	ContextProviderUnsubscribedCallback _tmp2;
	ContextProviderStringSet* _tmp4;
	ContextProviderStringSet* _tmp3;
	g_return_val_if_fail (keys != NULL, NULL);
	self = g_object_newv (object_type, 0, NULL);
	self->priv->get_cb = (_tmp0 = get_cb, self->priv->get_cb_target = get_cb_target, _tmp0);
	self->priv->first_cb = (_tmp1 = first_cb, self->priv->first_cb_target = first_cb_target, _tmp1);
	self->priv->last_cb = (_tmp2 = last_cb, self->priv->last_cb_target = last_cb_target, _tmp2);
	_tmp4 = NULL;
	_tmp3 = NULL;
	self->priv->keys = (_tmp4 = (_tmp3 = keys, (_tmp3 == NULL) ? NULL : context_provider_string_set_ref (_tmp3)), (self->priv->keys == NULL) ? NULL : (self->priv->keys = (context_provider_string_set_unref (self->priv->keys), NULL)), _tmp4);
	return self;
}


ContextProviderCProvider* context_provider_cprovider_new (ContextProviderStringSet* keys, ContextProviderGetCallback get_cb, void* get_cb_target, ContextProviderSubscribedCallback first_cb, void* first_cb_target, ContextProviderUnsubscribedCallback last_cb, void* last_cb_target) {
	return context_provider_cprovider_construct (CONTEXT_PROVIDER_TYPE_CPROVIDER, keys, get_cb, get_cb_target, first_cb, first_cb_target, last_cb, last_cb_target);
}


/* Implementation of the Provider interface by using callbacks*/
static void context_provider_cprovider_real_get (ContextProviderProvider* base, ContextProviderStringSet* keys, GHashTable* ret, GeeArrayList* unavail) {
	ContextProviderCProvider * self;
	char* _tmp0;
	self = (ContextProviderCProvider*) base;
	g_return_if_fail (keys != NULL);
	g_return_if_fail (ret != NULL);
	g_return_if_fail (unavail != NULL);
	_tmp0 = NULL;
	g_debug ("cprovider.vala:24: CProvider.get called with keys %s", _tmp0 = context_provider_string_set_debug (keys));
	_tmp0 = (g_free (_tmp0), NULL);
	if (self->priv->get_cb != NULL) {
		ContextProviderChangeSet* change_set;
		ContextProviderChangeSet* _tmp2;
		ContextProviderStringSet* _tmp1;
		change_set = context_provider_change_set_new_from_get (ret, unavail);
		_tmp2 = NULL;
		_tmp1 = NULL;
		self->priv->get_cb ((_tmp1 = keys, (_tmp1 == NULL) ? NULL : context_provider_string_set_ref (_tmp1)), (_tmp2 = change_set, (_tmp2 == NULL) ? NULL : context_provider_change_set_ref (_tmp2)), self->priv->get_cb_target);
		(change_set == NULL) ? NULL : (change_set = (context_provider_change_set_unref (change_set), NULL));
	}
}


static void context_provider_cprovider_real_keys_subscribed (ContextProviderProvider* base, ContextProviderStringSet* keys) {
	ContextProviderCProvider * self;
	self = (ContextProviderCProvider*) base;
	g_return_if_fail (keys != NULL);
	if (self->priv->first_cb != NULL) {
		self->priv->first_cb (keys, self->priv->first_cb_target);
	}
}


static void context_provider_cprovider_real_keys_unsubscribed (ContextProviderProvider* base, ContextProviderStringSet* keys_unsubscribed, ContextProviderStringSet* keys_remaining) {
	ContextProviderCProvider * self;
	self = (ContextProviderCProvider*) base;
	g_return_if_fail (keys_unsubscribed != NULL);
	g_return_if_fail (keys_remaining != NULL);
	if (self->priv->last_cb != NULL) {
		self->priv->last_cb (keys_unsubscribed, keys_remaining, self->priv->last_cb_target);
	}
}


static ContextProviderStringSet* context_provider_cprovider_real_provided_keys (ContextProviderProvider* base) {
	ContextProviderCProvider * self;
	ContextProviderStringSet* _tmp0;
	self = (ContextProviderCProvider*) base;
	_tmp0 = NULL;
	return (_tmp0 = self->priv->keys, (_tmp0 == NULL) ? NULL : context_provider_string_set_ref (_tmp0));
}


static void context_provider_cprovider_class_init (ContextProviderCProviderClass * klass) {
	context_provider_cprovider_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (ContextProviderCProviderPrivate));
	G_OBJECT_CLASS (klass)->finalize = context_provider_cprovider_finalize;
}


static void context_provider_cprovider_context_provider_provider_interface_init (ContextProviderProviderIface * iface) {
	context_provider_cprovider_context_provider_provider_parent_iface = g_type_interface_peek_parent (iface);
	iface->get = context_provider_cprovider_real_get;
	iface->keys_subscribed = context_provider_cprovider_real_keys_subscribed;
	iface->keys_unsubscribed = context_provider_cprovider_real_keys_unsubscribed;
	iface->provided_keys = context_provider_cprovider_real_provided_keys;
}


static void context_provider_cprovider_instance_init (ContextProviderCProvider * self) {
	self->priv = CONTEXT_PROVIDER_CPROVIDER_GET_PRIVATE (self);
}


static void context_provider_cprovider_finalize (GObject* obj) {
	ContextProviderCProvider * self;
	self = CONTEXT_PROVIDER_CPROVIDER (obj);
	(self->priv->keys == NULL) ? NULL : (self->priv->keys = (context_provider_string_set_unref (self->priv->keys), NULL));
	G_OBJECT_CLASS (context_provider_cprovider_parent_class)->finalize (obj);
}


GType context_provider_cprovider_get_type (void) {
	static GType context_provider_cprovider_type_id = 0;
	if (context_provider_cprovider_type_id == 0) {
		static const GTypeInfo g_define_type_info = { sizeof (ContextProviderCProviderClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) context_provider_cprovider_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ContextProviderCProvider), 0, (GInstanceInitFunc) context_provider_cprovider_instance_init, NULL };
		static const GInterfaceInfo context_provider_provider_info = { (GInterfaceInitFunc) context_provider_cprovider_context_provider_provider_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		context_provider_cprovider_type_id = g_type_register_static (G_TYPE_OBJECT, "ContextProviderCProvider", &g_define_type_info, 0);
		g_type_add_interface_static (context_provider_cprovider_type_id, CONTEXT_PROVIDER_TYPE_PROVIDER, &context_provider_provider_info);
	}
	return context_provider_cprovider_type_id;
}




