
#ifndef __PROVIDERS_H__
#define __PROVIDERS_H__

#include <glib.h>
#include <glib-object.h>
#include <gee/arraylist.h>
#include <string_set.h>
#include <provider_interface.h>

G_BEGIN_DECLS


#define CONTEXT_PROVIDER_TYPE_PROVIDERS (context_provider_providers_get_type ())
#define CONTEXT_PROVIDER_PROVIDERS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONTEXT_PROVIDER_TYPE_PROVIDERS, ContextProviderProviders))
#define CONTEXT_PROVIDER_PROVIDERS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), CONTEXT_PROVIDER_TYPE_PROVIDERS, ContextProviderProvidersClass))
#define CONTEXT_PROVIDER_IS_PROVIDERS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CONTEXT_PROVIDER_TYPE_PROVIDERS))
#define CONTEXT_PROVIDER_IS_PROVIDERS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CONTEXT_PROVIDER_TYPE_PROVIDERS))
#define CONTEXT_PROVIDER_PROVIDERS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), CONTEXT_PROVIDER_TYPE_PROVIDERS, ContextProviderProvidersClass))

typedef struct _ContextProviderProviders ContextProviderProviders;
typedef struct _ContextProviderProvidersClass ContextProviderProvidersClass;
typedef struct _ContextProviderProvidersPrivate ContextProviderProvidersPrivate;
typedef struct _ContextProviderParamSpecProviders ContextProviderParamSpecProviders;

struct _ContextProviderProviders {
	GTypeInstance parent_instance;
	volatile int ref_count;
	ContextProviderProvidersPrivate * priv;
};

struct _ContextProviderProvidersClass {
	GTypeClass parent_class;
	void (*finalize) (ContextProviderProviders *self);
};

struct _ContextProviderParamSpecProviders {
	GParamSpec parent_instance;
};


ContextProviderProviders* context_provider_providers_construct (GType object_type);
ContextProviderProviders* context_provider_providers_new (void);
void context_provider_providers_register (ContextProviderProviders* self, ContextProviderProvider* provider);
void context_provider_providers_unregister (ContextProviderProviders* self, ContextProviderProvider* provider);
GeeArrayList* context_provider_providers_get (ContextProviderProviders* self, ContextProviderStringSet* keys, GHashTable* values);
void context_provider_providers_first_subscribed (ContextProviderProviders* self, ContextProviderStringSet* keys);
void context_provider_providers_last_unsubscribed (ContextProviderProviders* self, ContextProviderStringSet* keys_unsubscribed, ContextProviderStringSet* keys_remaining);
ContextProviderStringSet* context_provider_providers_get_valid_keys (ContextProviderProviders* self);
GParamSpec* context_provider_param_spec_providers (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags);
gpointer context_provider_value_get_providers (const GValue* value);
void context_provider_value_set_providers (GValue* value, gpointer v_object);
GType context_provider_providers_get_type (void);
gpointer context_provider_providers_ref (gpointer instance);
void context_provider_providers_unref (gpointer instance);


G_END_DECLS

#endif
