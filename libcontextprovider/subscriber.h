
#ifndef __SUBSCRIBER_H__
#define __SUBSCRIBER_H__

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <gee/arraylist.h>
#include <dbus_interface.h>
#include <manager.h>
#include <key_usage_counter.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>

G_BEGIN_DECLS


#define CONTEXT_PROVIDER_TYPE_SUBSCRIBER (context_provider_subscriber_get_type ())
#define CONTEXT_PROVIDER_SUBSCRIBER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONTEXT_PROVIDER_TYPE_SUBSCRIBER, ContextProviderSubscriber))
#define CONTEXT_PROVIDER_SUBSCRIBER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), CONTEXT_PROVIDER_TYPE_SUBSCRIBER, ContextProviderSubscriberClass))
#define CONTEXT_PROVIDER_IS_SUBSCRIBER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CONTEXT_PROVIDER_TYPE_SUBSCRIBER))
#define CONTEXT_PROVIDER_IS_SUBSCRIBER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CONTEXT_PROVIDER_TYPE_SUBSCRIBER))
#define CONTEXT_PROVIDER_SUBSCRIBER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), CONTEXT_PROVIDER_TYPE_SUBSCRIBER, ContextProviderSubscriberClass))

typedef struct _ContextProviderSubscriber ContextProviderSubscriber;
typedef struct _ContextProviderSubscriberClass ContextProviderSubscriberClass;
typedef struct _ContextProviderSubscriberPrivate ContextProviderSubscriberPrivate;

struct _ContextProviderSubscriber {
	GObject parent_instance;
	ContextProviderSubscriberPrivate * priv;
};

struct _ContextProviderSubscriberClass {
	GObjectClass parent_class;
};


ContextProviderSubscriber* context_provider_subscriber_construct (GType object_type, ContextProviderManager* manager, ContextProviderKeyUsageCounter* key_counter, gint id);
ContextProviderSubscriber* context_provider_subscriber_new (ContextProviderManager* manager, ContextProviderKeyUsageCounter* key_counter, gint id);
void context_provider_subscriber_emit_changed (ContextProviderSubscriber* self, GHashTable* values, GeeArrayList* unavail_l);
void context_provider_subscriber_on_value_changed (ContextProviderSubscriber* self, GHashTable* changed_properties, GeeArrayList* changed_undeterminable);
const char* context_provider_subscriber_get_object_path (ContextProviderSubscriber* self);
void context_provider_subscriber_set_object_path (ContextProviderSubscriber* self, const char* value);
void context_provider_subscriber_dbus_register_object (DBusConnection* connection, const char* path, void* object);
DBusHandlerResult context_provider_subscriber_dbus_message (DBusConnection* connection, DBusMessage* message, void* object);
GType context_provider_subscriber_get_type (void);


G_END_DECLS

#endif
