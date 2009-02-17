
#ifndef __PROVIDER_INTERFACE_H__
#define __PROVIDER_INTERFACE_H__

#include <glib.h>
#include <glib-object.h>
#include <gee/arraylist.h>
#include <string_set.h>

G_BEGIN_DECLS


#define CONTEXT_PROVIDER_TYPE_PROVIDER (context_provider_provider_get_type ())
#define CONTEXT_PROVIDER_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONTEXT_PROVIDER_TYPE_PROVIDER, ContextProviderProvider))
#define CONTEXT_PROVIDER_IS_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CONTEXT_PROVIDER_TYPE_PROVIDER))
#define CONTEXT_PROVIDER_PROVIDER_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), CONTEXT_PROVIDER_TYPE_PROVIDER, ContextProviderProviderIface))

typedef struct _ContextProviderProvider ContextProviderProvider;
typedef struct _ContextProviderProviderIface ContextProviderProviderIface;

/**
 * ContextProviderProvider:
 *
 * Represents a provider of context
 */
struct _ContextProviderProviderIface {
	GTypeInterface parent_iface;
	void (*get) (ContextProviderProvider* self, ContextProviderStringSet* keys, GHashTable* ret, GeeArrayList* unavailable_keys);
	void (*keys_subscribed) (ContextProviderProvider* self, ContextProviderStringSet* keys_subscribed);
	void (*keys_unsubscribed) (ContextProviderProvider* self, ContextProviderStringSet* keys_unsubscribed, ContextProviderStringSet* keys_left);
	ContextProviderStringSet* (*provided_keys) (ContextProviderProvider* self);
};


void context_provider_provider_get (ContextProviderProvider* self, ContextProviderStringSet* keys, GHashTable* ret, GeeArrayList* unavailable_keys);
void context_provider_provider_keys_subscribed (ContextProviderProvider* self, ContextProviderStringSet* keys_subscribed);
void context_provider_provider_keys_unsubscribed (ContextProviderProvider* self, ContextProviderStringSet* keys_unsubscribed, ContextProviderStringSet* keys_left);
ContextProviderStringSet* context_provider_provider_provided_keys (ContextProviderProvider* self);
GType context_provider_provider_get_type (void);


G_END_DECLS

#endif
