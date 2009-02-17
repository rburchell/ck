
#ifndef __DBUS_INTERFACE_H__
#define __DBUS_INTERFACE_H__

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>

G_BEGIN_DECLS


#define CONTEXT_PROVIDER_TYPE_DBUS_MANAGER (context_provider_dbus_manager_get_type ())
#define CONTEXT_PROVIDER_DBUS_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONTEXT_PROVIDER_TYPE_DBUS_MANAGER, ContextProviderDBusManager))
#define CONTEXT_PROVIDER_IS_DBUS_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CONTEXT_PROVIDER_TYPE_DBUS_MANAGER))
#define CONTEXT_PROVIDER_DBUS_MANAGER_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), CONTEXT_PROVIDER_TYPE_DBUS_MANAGER, ContextProviderDBusManagerIface))

typedef struct _ContextProviderDBusManager ContextProviderDBusManager;
typedef struct _ContextProviderDBusManagerIface ContextProviderDBusManagerIface;

#define CONTEXT_PROVIDER_TYPE_DBUS_SUBSCRIBER (context_provider_dbus_subscriber_get_type ())
#define CONTEXT_PROVIDER_DBUS_SUBSCRIBER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONTEXT_PROVIDER_TYPE_DBUS_SUBSCRIBER, ContextProviderDBusSubscriber))
#define CONTEXT_PROVIDER_IS_DBUS_SUBSCRIBER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CONTEXT_PROVIDER_TYPE_DBUS_SUBSCRIBER))
#define CONTEXT_PROVIDER_DBUS_SUBSCRIBER_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), CONTEXT_PROVIDER_TYPE_DBUS_SUBSCRIBER, ContextProviderDBusSubscriberIface))

typedef struct _ContextProviderDBusSubscriber ContextProviderDBusSubscriber;
typedef struct _ContextProviderDBusSubscriberIface ContextProviderDBusSubscriberIface;

struct _ContextProviderDBusManagerIface {
	GTypeInterface parent_iface;
	void (*Get) (ContextProviderDBusManager* self, char** keys, int keys_length1, GHashTable** values, char*** undeterminable_keys, int* undeterminable_keys_length1);
	char* (*GetSubscriber) (ContextProviderDBusManager* self, const char* name, GError** error);
};

struct _ContextProviderDBusSubscriberIface {
	GTypeInterface parent_iface;
	void (*Subscribe) (ContextProviderDBusSubscriber* self, char** keys, int keys_length1, GHashTable** values, char*** undeterminable_keys, int* undeterminable_keys_length1);
	void (*Unsubscribe) (ContextProviderDBusSubscriber* self, char** keys, int keys_length1);
};


void context_provider_dbus_manager_Get (ContextProviderDBusManager* self, char** keys, int keys_length1, GHashTable** values, char*** undeterminable_keys, int* undeterminable_keys_length1);
char* context_provider_dbus_manager_GetSubscriber (ContextProviderDBusManager* self, const char* name, GError** error);
void context_provider_dbus_manager_dbus_register_object (DBusConnection* connection, const char* path, void* object);
DBusHandlerResult context_provider_dbus_manager_dbus_message (DBusConnection* connection, DBusMessage* message, void* object);
GType context_provider_dbus_manager_get_type (void);
ContextProviderDBusManager* context_provider_dbus_manager_dbus_proxy_new (DBusGConnection* connection, const char* name, const char* path);
void context_provider_dbus_subscriber_Subscribe (ContextProviderDBusSubscriber* self, char** keys, int keys_length1, GHashTable** values, char*** undeterminable_keys, int* undeterminable_keys_length1);
void context_provider_dbus_subscriber_Unsubscribe (ContextProviderDBusSubscriber* self, char** keys, int keys_length1);
void context_provider_dbus_subscriber_dbus_register_object (DBusConnection* connection, const char* path, void* object);
DBusHandlerResult context_provider_dbus_subscriber_dbus_message (DBusConnection* connection, DBusMessage* message, void* object);
GType context_provider_dbus_subscriber_get_type (void);
ContextProviderDBusSubscriber* context_provider_dbus_subscriber_dbus_proxy_new (DBusGConnection* connection, const char* name, const char* path);


G_END_DECLS

#endif
