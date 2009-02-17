
#ifndef __CPROVIDER_H__
#define __CPROVIDER_H__

#include <glib.h>
#include <glib-object.h>
#include <gee/arraylist.h>
#include <provider_interface.h>
#include <string_set.h>
#include <context_provider.h>

G_BEGIN_DECLS


#define CONTEXT_PROVIDER_TYPE_CPROVIDER (context_provider_cprovider_get_type ())
#define CONTEXT_PROVIDER_CPROVIDER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONTEXT_PROVIDER_TYPE_CPROVIDER, ContextProviderCProvider))
#define CONTEXT_PROVIDER_CPROVIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), CONTEXT_PROVIDER_TYPE_CPROVIDER, ContextProviderCProviderClass))
#define CONTEXT_PROVIDER_IS_CPROVIDER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CONTEXT_PROVIDER_TYPE_CPROVIDER))
#define CONTEXT_PROVIDER_IS_CPROVIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CONTEXT_PROVIDER_TYPE_CPROVIDER))
#define CONTEXT_PROVIDER_CPROVIDER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), CONTEXT_PROVIDER_TYPE_CPROVIDER, ContextProviderCProviderClass))

typedef struct _ContextProviderCProvider ContextProviderCProvider;
typedef struct _ContextProviderCProviderClass ContextProviderCProviderClass;
typedef struct _ContextProviderCProviderPrivate ContextProviderCProviderPrivate;

struct _ContextProviderCProvider {
	GObject parent_instance;
	ContextProviderCProviderPrivate * priv;
};

struct _ContextProviderCProviderClass {
	GObjectClass parent_class;
};


ContextProviderCProvider* context_provider_cprovider_construct (GType object_type, ContextProviderStringSet* keys, ContextProviderGetCallback get_cb, void* get_cb_target, ContextProviderSubscribedCallback first_cb, void* first_cb_target, ContextProviderUnsubscribedCallback last_cb, void* last_cb_target);
ContextProviderCProvider* context_provider_cprovider_new (ContextProviderStringSet* keys, ContextProviderGetCallback get_cb, void* get_cb_target, ContextProviderSubscribedCallback first_cb, void* first_cb_target, ContextProviderUnsubscribedCallback last_cb, void* last_cb_target);
GType context_provider_cprovider_get_type (void);


G_END_DECLS

#endif
