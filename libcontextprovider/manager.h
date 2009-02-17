
#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib.h>
#include <gee/arraylist.h>
#include <dbus_interface.h>
#include <providers.h>
#include <key_usage_counter.h>
#include <string_set.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>

G_BEGIN_DECLS


#define CONTEXT_PROVIDER_TYPE_MANAGER (context_provider_manager_get_type ())
#define CONTEXT_PROVIDER_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONTEXT_PROVIDER_TYPE_MANAGER, ContextProviderManager))
#define CONTEXT_PROVIDER_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), CONTEXT_PROVIDER_TYPE_MANAGER, ContextProviderManagerClass))
#define CONTEXT_PROVIDER_IS_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CONTEXT_PROVIDER_TYPE_MANAGER))
#define CONTEXT_PROVIDER_IS_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CONTEXT_PROVIDER_TYPE_MANAGER))
#define CONTEXT_PROVIDER_MANAGER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), CONTEXT_PROVIDER_TYPE_MANAGER, ContextProviderManagerClass))

typedef struct _ContextProviderManager ContextProviderManager;
typedef struct _ContextProviderManagerClass ContextProviderManagerClass;
typedef struct _ContextProviderManagerPrivate ContextProviderManagerPrivate;

struct _ContextProviderManager {
	GObject parent_instance;
	ContextProviderManagerPrivate * priv;
};

struct _ContextProviderManagerClass {
	GObjectClass parent_class;
};


ContextProviderManager* context_provider_manager_get_instance (void);
void context_provider_manager_set_bus_type (DBusBusType b);
void context_provider_manager_get_internal (ContextProviderManager* self, ContextProviderStringSet* keyset, GHashTable** values_to_send, char*** undeterminable_keys, int* undeterminable_keys_length1);
ContextProviderStringSet* context_provider_manager_check_keys (ContextProviderManager* self, char** keys, int keys_length1);
gboolean context_provider_manager_property_values_changed (ContextProviderManager* self, GHashTable* properties, GeeArrayList* undeterminable_keys);
ContextProviderProviders* context_provider_manager_get_providers (ContextProviderManager* self);
ContextProviderKeyUsageCounter* context_provider_manager_get_key_counter (ContextProviderManager* self);
void context_provider_manager_dbus_register_object (DBusConnection* connection, const char* path, void* object);
DBusHandlerResult context_provider_manager_dbus_message (DBusConnection* connection, DBusMessage* message, void* object);
GType context_provider_manager_get_type (void);


G_END_DECLS

#endif
